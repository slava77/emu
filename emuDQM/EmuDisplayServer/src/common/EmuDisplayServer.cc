#include <iostream>
#include <string>
#include <sstream>
#include "EmuDisplayServer.h"
#include "I2O_DQMDataRequest.h"

#include "xdaq.h"
#include "xgi/Method.h"
#include "i2o/Method.h"
#include "i2o/utils/AddressMap.h"
#include "pt/PeerTransportAgent.h"
#include "xoap/Method.h"
#include "xdaq/NamespaceURI.h"
#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPPart.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"
#include "xcept/tools.h"



#include "TBuffer.h"
#include "TMessage.h"
#include "TObject.h"
#include "TH1F.h"
#include "TH1.h"
#include "TCanvas.h"
#include "THStack.h"
    
using namespace std;

XDAQ_INSTANTIATOR_IMPL(EmuDisplayServer)

EmuDisplayServer::EmuDisplayServer(xdaq::ApplicationStub* stub)
        throw (xdaq::exception::Exception)
        : xdaq::WebApplication(stub)
{

  RefFileName = "/csc_data/root/dqm_ref.root";
  consinfo = new TConsumerInfo("DQM", 1);
  userServer = new EmuUserDisplayServerTask();
  RefServer = new EmuRefDisplayServerTask(); 
    //Ref1Server(new EmuRef1DisplayServerTask())

    getApplicationInfoSpace()->fireItemAvailable("RefFileName",&RefFileName);
//     exportParam("Ref File Name", RefFileName);
//    i2o::bind (this, &EmuDisplayServer::I2O_DQMDataRequest, I2O_DQMDataRequestMsgCode, XDAQ_ORGANIZATION_ID);
        errorHandler_ = toolbox::exception::bind (this, &EmuDisplayServer::onError, "onError");

	fsm_.addState ('H', "Halted");
        fsm_.addState ('R', "Ready");
        fsm_.addState ('E', "Enabled");
        fsm_.addStateTransition ('H','R', "Configure", this, &EmuDisplayServer::ConfigureAction);
        fsm_.addStateTransition ('R','E', "Enable", this, &EmuDisplayServer::EnableAction);
        fsm_.addStateTransition ('E','H', "Halt", this, &EmuDisplayServer::HaltAction);
        fsm_.addStateTransition ('R','H', "Halt", this, &EmuDisplayServer::HaltAction);
        fsm_.setInitialState('H');
        fsm_.reset();

	// Bind SOAP callbacks for control messages
        xoap::bind (this, &EmuDisplayServer::fireEvent, "Configure", XDAQ_NS_URI);
        xoap::bind (this, &EmuDisplayServer::fireEvent, "Enable", XDAQ_NS_URI);
        xoap::bind (this, &EmuDisplayServer::fireEvent, "Halt", XDAQ_NS_URI);

        // Bind CGI callbacks
        xgi::bind(this, &EmuDisplayServer::dispatch, "dispatch");

        // Define Web state machine
        wsm_.addState('H', "Halted",    this, &EmuDisplayServer::stateMachinePage);
        wsm_.addState('R', "Ready",     this, &EmuDisplayServer::stateMachinePage);
        wsm_.addState('E', "Enabled",   this, &EmuDisplayServer::stateMachinePage);
        wsm_.addStateTransition('H','R', "Configure", this, &EmuDisplayServer::Configure, &EmuDisplayServer::failurePage);
        wsm_.addStateTransition('R','E', "Enable",    this, &EmuDisplayServer::Enable,    &EmuDisplayServer::failurePage);
        wsm_.addStateTransition('R','H', "Halt",      this, &EmuDisplayServer::Halt,      &EmuDisplayServer::failurePage);
        wsm_.addStateTransition('E','H', "Halt",      this, &EmuDisplayServer::Halt,      &EmuDisplayServer::failurePage);
        wsm_.setInitialState('H');

	xoap::bind (this, &EmuDisplayServer::updateList, "updateList", XDAQ_NS_URI);
        xoap::bind (this, &EmuDisplayServer::updateObjects, "updateObjects", XDAQ_NS_URI);


    nodelist.clear(); 
    objlist.clear();
}
    
    
EmuDisplayServer::~EmuDisplayServer() 
{
    userServer->kill();
    RefServer->kill();
    //Ref1Server->kill();
    delete userServer;
    delete RefServer;
    //delete Ref1Server;
    delete consinfo;
}


//
// SOAP Callback trigger state change
//
xoap::MessageReference EmuDisplayServer::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
        xoap::SOAPPart part = msg->getSOAPPart();
        xoap::SOAPEnvelope env = part.getEnvelope();
        xoap::SOAPBody body = env.getBody();
        DOMNode* node = body.getDOMNode();
        DOMNodeList* bodyList = node->getChildNodes();
        for (unsigned int i = 0; i < bodyList->getLength(); i++)
        {
                DOMNode* command = bodyList->item(i);

                if (command->getNodeType() == DOMNode::ELEMENT_NODE)
                {
                        std::string commandName = xoap::XMLCh2String (command->getLocalName());

                        try
                        {
                                toolbox::Event::Reference e(new toolbox::Event(commandName, this));
                                fsm_.fireEvent(e);
                                // Synchronize Web state machine
                                wsm_.setInitialState(fsm_.getCurrentState());
                        }
                        catch (toolbox::fsm::exception::Exception & e)
                        {
                                XCEPT_RETHROW(xcept::Exception, "invalid command", e);
                        }

                        xoap::MessageReference reply = xoap::createMessage();
                        xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
                        xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
                        envelope.getBody().addBodyElement ( responseName );
                        return reply;
                }
        }

        XCEPT_RAISE(xoap::exception::Exception,"command not found");
}

bool EmuDisplayServer::onError ( xcept::Exception& ex, void * context )
{
        std::cout << "onError: " << ex.what() << std::endl;
        return false;
}


void EmuDisplayServer::ConfigureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception) 
{
    userServer->activate();
    LOG4CPLUS_INFO (getApplicationLogger(),"Configure EmuDisplayServer for DQM Histos");
    RefServer->activate();
    LOG4CPLUS_INFO (getApplicationLogger(),"Configure EmuDisplayServer for Ref Histos");
    //Ref1Server->activate();
    //cout << "+++ Configure EmuDisplayServer for Dynamic Ref Histos" << endl;
    RFile = TFile::Open(RefFileName.toString().c_str(), "READ");
    if(RFile) { LOG4CPLUS_DEBUG (getApplicationLogger(),toolbox::toString("Reference Histogram File Opened: %s", RFile->GetName())); }
    else {LOG4CPLUS_ERROR (getApplicationLogger(),toolbox::toString("Could not open Reference Histogram File : %s", RefFileName.toString())); }
    InitialiseRef0 = 0;
}

void EmuDisplayServer::EnableAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
    // userServer->activate();
    LOG4CPLUS_INFO (getApplicationLogger(), "Enable EmuDisplayServer");
}

void EmuDisplayServer::HaltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
    userServer->kill();
    RefServer->kill();
    //Ref1Server->kill();
    LOG4CPLUS_INFO (getApplicationLogger(), "Disable EmuDisplayServer");
    if(RFile) RFile->Close();
}

// XGI Call back

void EmuDisplayServer::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
        wsm_.displayPage(out);
}

// WSM Dispatcher function
void EmuDisplayServer::dispatch (xgi::Input * in, xgi::Output * out)  throw (xgi::exception::Exception)
{
        cgicc::Cgicc cgi(in);
        //const cgicc::CgiEnvironment& env = cgi.getEnvironment();
        cgicc::const_form_iterator stateInputElement = cgi.getElement("StateInput");
        std::string stateInput = (*stateInputElement).getValue();
        wsm_.fireEvent(stateInput,in,out);
}


//
// Web Events that trigger state changes (result of wsm::fireEvent)
//
void EmuDisplayServer::Configure(xgi::Input * in ) throw (xgi::exception::Exception)
{
        try
        {
                toolbox::Event::Reference e(new toolbox::Event("Configure", this));
                fsm_.fireEvent(e);
        }
        catch (toolbox::fsm::exception::Exception & e)
        {
                XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
        }
}

void EmuDisplayServer::Enable(xgi::Input * in ) throw (xgi::exception::Exception)
{
        try
        {
                toolbox::Event::Reference e(new toolbox::Event("Enable", this));
                fsm_.fireEvent(e);
        }
        catch (toolbox::fsm::exception::Exception & e)
        {
                XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
        }
}

void EmuDisplayServer::Halt(xgi::Input * in ) throw (xgi::exception::Exception)
{
        try
        {
                toolbox::Event::Reference e(new toolbox::Event("Halt", this));
                fsm_.fireEvent(e);
        }
        catch (toolbox::fsm::exception::Exception & e)
        {
                XCEPT_RETHROW(xgi::exception::Exception, "invalid command", e);
        }
}

//
// Web Navigation Pages
//
void EmuDisplayServer::stateMachinePage( xgi::Output * out ) throw (xgi::exception::Exception)
{
        *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
        *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

        xgi::Utils::getPageHeader
                (out,
                "EmuDisplayServer",
                getApplicationDescriptor()->getContextDescriptor()->getURL(),
                getApplicationDescriptor()->getURN(),
                "/daq/xgi/images/Application.gif"
                );

        std::string url = "/";
        url += getApplicationDescriptor()->getURN();
        url += "/dispatch";

        // display FSM
        std::set<std::string> possibleInputs = wsm_.getInputs(wsm_.getCurrentState());
        std::set<std::string> allInputs = wsm_.getInputs();


        *out << cgicc::h3("Finite State Machine").set("style", "font-family: arial") << std::endl;
        *out << "<table border cellpadding=10 cellspacing=0>" << std::endl;
        *out << "<tr>" << std::endl;
        *out << "<th>" << wsm_.getStateName(wsm_.getCurrentState()) << "</th>" << std::endl;
        *out << "</tr>" << std::endl;
        *out << "<tr>" << std::endl;
        std::set<std::string>::iterator i;
        for ( i = allInputs.begin(); i != allInputs.end(); i++)
        {
                *out << "<td>";
                *out << cgicc::form().set("method","get").set("action", url).set("enctype","multipart/form-data") << std::endl;

                if ( possibleInputs.find(*i) != possibleInputs.end() )
                {
                        *out << cgicc::input().set("type", "submit").set("name", "StateInput").set("value", (*i) );
                }
                else
                {
                        *out << cgicc::input() .set("type", "submit").set("name", "StateInput").set("value", (*i) ).set("disabled", "true");
                }

                *out << cgicc::form();
                *out << "</td>" << std::endl;
        }
       *out << "</tr>" << std::endl;
        *out << "</table>" << std::endl;
        //

        *out << cgicc::hr() << std::endl;
/*
        std::string measurementURL = "/";
        measurementURL += getApplicationDescriptor()->getURN();
        measurementURL += "/displayMeasurements";

        *out << cgicc::br() << "<a href=\"" << measurementURL << "\">" << "Display Measurements" << "</a>" << cgicc::br() << endl;
*/

        xgi::Utils::getPageFooter(*out);
}

//
// Failure Pages
//
void EmuDisplayServer::failurePage(xgi::Output * out, xgi::exception::Exception & e)  throw (xgi::exception::Exception)
{
        *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
        *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;

        xgi::Utils::getPageHeader
                (out,
                "EmuDisplayServer Failure",
                getApplicationDescriptor()->getContextDescriptor()->getURL(),
                getApplicationDescriptor()->getURN(),
                "/daq/xgi/images/Application.gif"
                );

        *out << cgicc::br() << e.what() << cgicc::br() << endl;
        std::string url = "/";
        url += getApplicationDescriptor()->getURN();

        *out << cgicc::br() << "<a href=\"" << url << "\">" << "retry" << "</a>" << cgicc::br() << endl;

        xgi::Utils::getPageFooter(*out);
}






void EmuDisplayServer::I2O_DQMDataRequest(toolbox::mem::Reference * ref)  throw (i2o::exception::Exception) 
{
    LOG4CPLUS_DEBUG(getApplicationLogger(), "Receiving data in EmuDisplayServer");
    // assume there's only one frame in this fragment
    /*
    size_t frameHeaderSize = sizeof(I2O_RU_DATA_READY_MESSAGE_FRAME);
    char *frame = ref->data();

    I2O_MESSAGE_FRAME               *stdMsgFrame =
          (I2O_MESSAGE_FRAME *)              frame;
    I2O_PRIVATE_MESSAGE_FRAME       *pvtMsgFrame =
          (I2O_PRIVATE_MESSAGE_FRAME *)      frame;
    I2O_RU_DATA_READY_MESSAGE_FRAME *ruMsgFrame =
          (I2O_RU_DATA_READY_MESSAGE_FRAME *)frame;

    char *data = frame + frameHeaderSize;
    // the message size seems to have been passed in 16-bit words
    int dataSize = 2*stdMsgFrame->MessageSize  - frameHeaderSize;

    cout << "Initiator: " << stdMsgFrame->InitiatorAddress << endl;
    cout << "Data Size: " << dataSize << endl;
    
    
    TBuffer mesg(TBuffer::kWrite);
    mesg.Reset();
    mesg.WriteBuf(data, dataSize);
    
    mesg.Reset();
    mesg.SetReadMode();

    TConsumerInfo* newInfo = (TConsumerInfo*) mesg.ReadObjectAny(TConsumerInfo::Class());
    if (newInfo)
        newInfo->print();

    xdaq::frameFree(ref);
	*/
}

xoap::MessageReference EmuDisplayServer::updateList(xoap::MessageReference node) throw (xoap::exception::Exception)
{
    /*
    cout << endl;
    node.writeTo(cout);
    cout << endl;
    */
    /*
    SOAPName cmdTag ("updateList", "", "");
    vector<SOAPElement> content = node.getSOAPPart().getEnvelope().getBody().getChildElements (cmdTag);
    SOAPName originator ("originator", "", "");
    for (vector<SOAPElement>::iterator itr = content.begin();
                    itr != content.end();++itr) {
        
        xdaqInteger nodeAddr(0);
        nodeAddr.fromString(itr->getAttributeValue(originator));
        cout << "+++ Requesting list from Node:" << nodeAddr.toString() << endl;
        requestList(nodeAddr);  
              
    }
    */
    xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
    if (rb.hasFault() )
    {
        xoap::SOAPFault fault = rb.getFault();
        string errmsg = "DQMNode: ";
        errmsg += fault.getFaultString();
        XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } else {                            
        map<string, list<string> > bmap;
        list<string> olist;
        consinfo->clear();
        vector<xoap::SOAPElement> content = rb.getChildElements ();
        xoap::SOAPName nodeTag ("DQMNode", "", "");
        for (vector<xoap::SOAPElement>::iterator itr = content.begin();
                itr != content.end(); ++itr) {
            
            vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
            for (vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                    n_itr != nodeElement.end(); ++n_itr) {
                    
                stringstream stdir;
                bmap.clear();
                xoap::SOAPName branchTag ("Branch", "", "");
                
                vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );
                                        
                for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin(); 
                        b_itr != branchElement.end(); ++b_itr) {
                    
                    olist.clear();
                    stringstream stdir;
                    string dir="";
                    stdir.clear(); 
                    // dir = b_itr->getValue();
                    int id;
                    stdir << b_itr->getValue();
                    stdir >> id;
                    stdir.clear();
                    if (id) {			
                        stdir << "/CSC_" << ((id>>4)&0xFF) << "_" << (id&0xF); 
                        stdir >> dir;                                            
                    }
                    if (id == SLIDES_ID) {
                        dir = "/" + string(SLIDES);
                    }   
                    
                    dir = "DQMNode"+n_itr->getValue()+dir;
                    // cout << dir << endl;
                    xoap::SOAPName objTag("Obj", "", "");
                    
                    vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );
                    
                    for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
                            o_itr != objElement.end(); ++o_itr ) {
                        olist.push_back(o_itr->getValue());
                        // Problem with duplicates search 
                        if (!consinfo->hasEntry(TString(o_itr->getValue().c_str())) ||
                            (consinfo->hasEntry(TString(o_itr->getValue().c_str())) &&
                            (consinfo->getPath(TString(o_itr->getValue().c_str())) != TString(dir.c_str())))) 
                        {
			    
			    string value = o_itr->getValue();
			    int pos = value.rfind("/",value.size());
			    if (pos != string::npos) {
                            string name = value.substr(pos+1,value.size());
			    string path = dir+"/"+value.substr(0, pos+1);
			    //cout << "path:" << path << " name:" << name << endl;
                            consinfo->addObject(TString(name.c_str()), TString(path.c_str()), 0, NULL);
		            } else {
			    consinfo->addObject(TString(o_itr->getValue().c_str()), TString(dir.c_str()), 0, NULL);
			    }
                            olist.sort();
                            // cout << "List Item: "<< dir<<"/"<<o_itr->getValue()<<endl;
			    
                            TMessage* refbuf = new TMessage(kMESS_OBJECT); 
                            if(refbuf) refbuf->Reset();
                            else cout << " ++++++ Pointer to reference buffer is NULL in updateList()" << endl;
                            string objname = dir+"/"+o_itr->getValue();
                            MessageStorage *refstorage = new MessageStorage(objname.c_str());
                            if (refbuf && refstorage) 
                            {
                              refHistComparator( o_itr->getValue(), refbuf, refbuf, refbuf );  //Ricks test
                              refbuf->Reset();
                              refstorage->updateMessage(refbuf);
                              RefServer->addStorage(refstorage);  //writes info to stdout
                            }
                            else
                              cout << " ++++++ Pointer to reference buffer or reference storage is NULL in updateList()" << endl;
			    
                        }
                    }
                    bmap[b_itr->getValue()] = olist;
                }
                objlist[n_itr->getValue()] = bmap;                  
            }
        }
    }
    cout << "+++ Consumer List is updated" <<endl;
    if (consinfo) 
    { 
      userServer->setInfo(*consinfo); consinfo->print(); cout << "Set Info for userServer" << endl;
      RefServer->setInfo(*consinfo); consinfo->print(); cout << "Set Info for RefServer" << endl;
      //Ref1Server->setInfo(*consinfo); consinfo->print(); cout << "Set Info for Ref1Server" << endl;
    }
    //   return reply;
    
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPBody b = envelope.getBody();
    xoap::SOAPName replyName = envelope.createName("updateListReply", "", "");
    xoap::SOAPBodyElement e = b.addBodyElement ( replyName );
    xoap::SOAPName statusName = envelope.createName("Status", "", "");
    xoap::SOAPElement statusElement = e.addChildElement(statusName);
    statusElement.addTextNode("LIST_UPDATED");
    //cout << endl;
    //msg.writeTo(cout);
    //cout << endl;
    return reply;
}

void EmuDisplayServer::requestList(xdata::Integer nodeaddr)
{

    // Prepare SOAP Message for DQM Mode 
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName commandName = envelope.createName("sendList");
    xoap::SOAPName originator ("originator", "", "");
    xoap::SOAPName targetAddr = envelope.createName("targetAddr");
    xoap::SOAPElement command = body.addBodyElement(commandName );
    // xdata::Integer localTid  (tid());
    // xdaqInteger targetTid (nodeaddr);
    // command.addAttribute (targetAddr, nodeaddr.toString());
    // command.addAttribute (originator, targetTid.toString() );
    
    // Get reply from DQM node and populate TConsumerInfo list
	try
        {
		xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
                xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);



/*    
    cout << endl;
    reply.writeTo(cout);
    cout << endl;
*/    
    xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
    if (rb.hasFault() )
    {
        xoap::SOAPFault fault = rb.getFault();
        string errmsg = "DQMNode: ";
        errmsg += fault.getFaultString();
        XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } else {                            
        map<string, list<string> > bmap;
        list<string> olist;
        consinfo->clear();
        vector<xoap::SOAPElement> content = rb.getChildElements ();
        xoap::SOAPName nodeTag ("DQMNode", "", "");
        for (vector<xoap::SOAPElement>::iterator itr = content.begin();
                itr != content.end(); ++itr) {
            
            vector<xoap::SOAPElement> nodeElement = content[0].getChildElements (nodeTag);
            
            for (vector<xoap::SOAPElement>::iterator n_itr = nodeElement.begin();
                    n_itr != nodeElement.end(); ++n_itr) {
                    
                stringstream stdir;
                bmap.clear();
                xoap::SOAPName branchTag ("Branch", "", "");
                
                vector<xoap::SOAPElement> branchElement = n_itr->getChildElements (branchTag );
                                        
                for (vector<xoap::SOAPElement>::iterator b_itr = branchElement.begin(); 
                        b_itr != branchElement.end(); ++b_itr) {
                    
                    olist.clear();
                    stringstream stdir;
                    string dir="";
                    stdir.clear(); 
                    // dir = b_itr->getValue();
                    
                    if (b_itr->getValue() != "0") {			
                        int id;
                        stdir << b_itr->getValue();
                        stdir >> id;
                        stdir.clear();                          
                        stdir << "/CSC_" << ((id>>4)&0xFF) << "_" << (id&0xF); 
                        stdir >> dir;                        
                    }
                    
                    dir = "DQMNode"+n_itr->getValue()+dir;
                    // cout << dir << endl;
                    xoap::SOAPName objTag("Obj", "", "");
                    
                    vector<xoap::SOAPElement> objElement = b_itr->getChildElements(objTag );
                    
                    for (vector<xoap::SOAPElement>::iterator o_itr = objElement.begin();
                            o_itr != objElement.end(); ++o_itr ) {
                        olist.push_back(o_itr->getValue());
                        // Problem with duplicates search 
                        if (!consinfo->hasEntry(TString(o_itr->getValue().c_str())) ||
                            (consinfo->hasEntry(TString(o_itr->getValue().c_str())) &&
                            (consinfo->getPath(TString(o_itr->getValue().c_str())) != TString(dir.c_str())))) 
                        {
  	                    /*
			    string value(o_itr->getValue());
                            string name(value.substr(value.rfind("/",value.size()),value.size()));
                            string path(dir+value.substr(0, value.rfind("/",value.size())));
                            consinfo->addObject(TString(name.c_str()), TString(path.c_str()), 0, NULL);	
			    */
                            consinfo->addObject(TString(o_itr->getValue().c_str()), TString(dir.c_str()), 0, NULL);
                            olist.sort();
                        }
                    }
                    bmap[b_itr->getValue()] = olist;
                }
                objlist[n_itr->getValue()] = bmap;                  
            }
        }
    }
    }
        catch (xdaq::exception::Exception& e)
        {
     // handle exception
        }

    cout << "+++ Consumer List is updated from requestList" <<endl;
    if (consinfo) userServer->setInfo(*consinfo);
    if (consinfo) RefServer->setInfo(*consinfo);
    //if (consinfo) Ref1Server->setInfo(*consinfo);
    //   return reply;
}

void EmuDisplayServer::refHistComparator(string mystring, TMessage *buffer, TMessage *refbuffer, TMessage *ref1buffer)
{
  //cout << "+++ REF-HIST Comparator test" << " "<< mystring << endl;
  int offset = 0;
  while( offset < mystring.size() )
    {
      if(mystring.c_str()[offset] == '/') break;
      offset++;
    }
  if(mystring.c_str()[offset] == '/') offset++; //to remove the '/' symbol
  else offset = 0;                              //if no '/' symbol then no offset
  char removedDirName[200];
  int position = 0;
  while( position+offset < mystring.size() )
    {
       removedDirName[position] = mystring.c_str()[position+offset];
       position++;
    }
  removedDirName[position] = '\0';
  TObject *Ref = 0;
  if(RFile)  Ref = RFile->Get( removedDirName ); //be careful to do only once per unique removedDirName, else memory leak  
  else cout << "+++ Pointer to NULL Reference File!! Can not get object " << removedDirName << endl;
  /*
  buffer->SetReadMode();
  if( buffer ) 
  {
    if( buffer->What() == kMESS_OBJECT )
    {
      TObject* myobj = buffer->ReadObject(buffer->GetClass());
      if( myobj->InheritsFrom(TH1::Class()) ) 
      { 
        TH1 *Hist = (TH1*) myobj;
      }
      if( myobj->InheritsFrom(TCanvas::Class()) ) 
      { 
        TCanvas *Canvas = (TCanvas*) myobj;
        TPad *pad = (TPad*) Canvas;
        ExtractHistosFromCanvas( pad );
        TPad *RefPad = (TPad*) Ref;
        ExtractHistosFromCanvas( RefPad );
      }
    }
  }
  buffer->Reset();
  */
  refbuffer->SetWriteMode();  
  refbuffer->Reset();
  if( Ref && refbuffer )
    {
      if( refbuffer->What() == kMESS_OBJECT ) refbuffer->WriteObjectAny( Ref, Ref->Class() );
    }
  else
    cout << "+++ NULL Pointer to reference object or reference buffer. Can not write object to buffer!!!" << endl;
  refbuffer->Reset();  
  /*
  buffer->SetReadMode();
  ref1buffer->SetWriteMode();  
  if( ref1buffer ) 
    if( ref1buffer->What() == kMESS_OBJECT )
    {
      TCanvas *Ref1 = (TCanvas*) buffer->ReadObject( buffer->GetClass() );
    //TCanvas *Ref1 = (TCanvas*) RFile->Get( mystring.c_str() );
      TPad *pad = (TPad*) Ref1;
      TH1 *histo; 
      histo = ModifyRefHistosFromCanvas( pad );
      if( histo && TString(Ref->GetName()) == "57CSC_Efficiency" )
	{
            THStack hs( Ref->GetName(), Ref->GetName() );
            TH1F *h1;
            (TObject *) h1 = histo->Clone();
            h1->SetBinContent(2, histo->GetBinContent(2) );     
            h1->SetBinContent(3, histo->GetBinContent(2) );     
            h1->SetBinContent(4, histo->GetBinContent(2) );  
            h1->SetBinContent(5, histo->GetBinContent(2) );     
            h1->SetFillColor(kBlack);
            h1->SetLineColor(kBlack); 
            hs.Add(h1);                   
            hs.Add(histo);                  
            //TCanvas *c1 = Ref1;  //unable for some reason to draw back onto the original canvas...
            TCanvas *c1 = new TCanvas(Ref->GetName(), Ref->GetName(), 10, 10, 400, 400); 
            hs.Draw("nostack");
            ref1buffer->WriteObjectAny( c1, c1->Class() );
            delete c1;
	}
      else 
	ref1buffer->WriteObjectAny( Ref1, Ref1->Class() );
    }
  ref1buffer->Reset();
  buffer->Reset();
  */  
}

void EmuDisplayServer::ExtractHistosFromCanvas( TVirtualPad * CurrentPad )
{
  TList *primitiveList = CurrentPad->GetListOfPrimitives();
  TIter next(primitiveList);
  TObject *obj = 0;
  while ( ( obj = next() ) ) 
  {
    if ( obj->InheritsFrom(TH1::Class()) ) 
    {
      const char *objname = obj->GetName();
    } 
    else if ( obj->InheritsFrom(TPad::Class()) ) 
    {
      ExtractHistosFromCanvas( (TPad*) obj );
    }  
  }
}

TH1* EmuDisplayServer::ModifyRefHistosFromCanvas( TVirtualPad * CurrentPad  )
{
  TList *primitiveList = CurrentPad->GetListOfPrimitives();
  TIter next(primitiveList);
  TObject *obj = 0;
  while ( ( obj = next() ) ) 
  {
    if ( obj->InheritsFrom(TH1::Class()) ) 
    {
      TH1 *h1 = (TH1*) obj;
      return h1;
    } 
    else if ( obj->InheritsFrom(TPad::Class()) ) 
    {
      return ModifyRefHistosFromCanvas( (TPad*) obj );
    }  
  }
  return 0;
}

xoap::MessageReference EmuDisplayServer::updateObjects(xoap::MessageReference node) throw (xoap::exception::Exception)
{
    /*
    cout << endl;
    node.writeTo(cout);
    cout << endl;
    */
    
    xoap::SOAPName cmdTag ("updateObjects", "", "");
    vector<xoap::SOAPElement> content = node->getSOAPPart().getEnvelope().getBody().getChildElements (cmdTag);
    xoap::SOAPName originator ("originator", "", "");
    xdata::Integer nodeAddr(0);
    for (vector<xoap::SOAPElement>::iterator itr = content.begin(); itr != content.end();++itr) 
    {        
        nodeAddr.fromString(itr->getAttributeValue(originator));
        //   requestList(nodeAddr);
        //cout << "+++ Requesting Objects from Node:" << nodeAddr.toString() << endl;    
        //requestObjects(nodeAddr); 
    }    
    list<xoap::AttachmentPart*> attachments = node->getAttachments();
    list<xoap::AttachmentPart*>::iterator iter;

    for (iter = attachments.begin(); iter != attachments.end(); iter++) 
    {
       int size = (*iter)->getSize();
       char * content = (*iter)->getContent();

       TMessage* buf = new TMessage(kMESS_OBJECT);
       //TMessage* ref1buf = new TMessage(kMESS_OBJECT);
       buf->Reset();
       //ref1buf->Reset();
       buf->WriteBuf(content, size);  
       //ref1buf->WriteBuf(content, size); 
       //cout << "+++ DQMNode" << nodeaddr.toString() << " Received " << obj->ClassName() <<" object: " << (*iter)->getContentLocation() << endl; 
       buf->Reset();
       //ref1buf->Reset();
       // MIME Headers functions does not return correct values
       map<string, string, less<string> > mimeHdrs = (*iter)->getAllMimeHeaders();       
       // cout << mimeHdrs["Content-Type"] << endl;
       // cout << mimeHdrs["Content-Location"] << endl;
       // cout << mimeHdrs["Content-Location"];
       // string objname="DQMNode"+nodeAddr.toString()+"/" + mimeHdrs["Content-Location"];
       string objname="DQMNode"+nodeAddr.toString()+"/" +(*iter)->getContentLocation();
       MessageStorage *storage = new MessageStorage(objname.c_str());
       //MessageStorage *ref1storage = new MessageStorage(objname.c_str());
       // cout << "++++++++++++++++ UpdateObjects: buf->GetClass = " << buf->GetClass() << endl;
       buf->Reset();
       //ref1buf->Reset();
       storage->updateMessage(buf);
       //ref1storage->updateMessage(ref1buf);
       userServer->addStorage(storage);    //writes info to stdout
       //Ref1Server->addStorage(ref1storage); //writes info to stdout
       // cout << objname <<":"<<size << " "<< storage->GetName() << ":" << storage->getMessage()->BufferSize() << endl;
       //delete buf;               // Memory leak in the original code!!!
       //delete storage;           // crash...forced to do the same
    }  
                              
    cout << "+++ Objects are updated" <<endl;
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
    xoap::SOAPBody b = envelope.getBody();
    xoap::SOAPName replyName = envelope.createName("updateObjectsReply", "", "");
    xoap::SOAPBodyElement e = b.addBodyElement ( replyName );
    xoap::SOAPName statusName = envelope.createName("UpdateStatus", "", "");
    xoap::SOAPElement statusElement = e.addChildElement(statusName);
    statusElement.addTextNode("OBJECTS_UPDATED");

    return reply;
}

void EmuDisplayServer::requestObjects(xdata::Integer nodeaddr)
{
    // Prepare SOAP Message for DQM Mode 
    xoap::MessageReference msg = xoap::createMessage();
    xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
    xoap::SOAPBody body = envelope.getBody();
    xoap::SOAPName commandName = envelope.createName("sendList");
    xoap::SOAPName originator ("originator", "", "");
    xoap::SOAPName targetAddr = envelope.createName("targetAddr");
    xoap::SOAPElement command = body.addBodyElement(commandName );
    // xdata::Integer localTid  (tid());
    // command.addAttribute (targetAddr, nodeaddr.toString() );
    // command.addAttribute (originator, localTid.toString() );
    
    // Get reply from DQM node and populate TConsumerInfo list
    
	try
	{
		xdaq::ApplicationDescriptor* d = i2o::utils::getAddressMap()->getApplicationDescriptor(nodeaddr);
     		xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);
 	} 
 	catch (xdaq::exception::Exception& e)
 	{
     // handle exception
 	}	
 
    /*
    cout << endl;
    reply.writeTo(cout);
    cout << endl;
    SOAPBody rb = reply.getSOAPPart().getEnvelope().getBody();
    if (rb.hasFault() )
    {
        SOAPFault fault = rb.getFault();
        string errmsg = "DQMNode: ";
        errmsg += fault.getFaultString();
        XDAQ_RAISE (xdaqException, errmsg);
    } else {  
           
        list<AttachmentPart*> attachments = reply.getAttachments();
        list<AttachmentPart*>::iterator iter;
        for (iter = attachments.begin(); iter != attachments.end(); iter++) {
            int size = (*iter)->getSize();
            char * content = (*iter)->getContent();
            TMessage* buf = new TMessage(kMESS_OBJECT);
            buf->Reset();
            buf->WriteBuf(content, size);         
            // cout << "+++ DQMNode" << nodeaddr.toString() << " Received " << obj->ClassName() <<" object: " << (*iter)->getContentLocation() << endl; 
            buf->Reset();
            string objname="DQMNode"+nodeaddr.toString()+"/"+(*iter)->getMimeHeader("Content-Description");
            // (*iter)->getContentLocation();
            cout << (*iter)->getMimeHeader("Content-Description") << endl;
            // cout << objname << endl;
            MessageStorage *storage = new MessageStorage(objname.c_str());
            storage->updateMessage(buf);
            userServer->addStorage(storage);    
            //cout << objname <<":"<<size << " "<< storage->GetName() << ":" << storage->getMessage()->BufferSize() << endl;
            delete buf;
        }  
                              
    } */
    cout << "+++ Objects are updated" <<endl;
    // if (consinfo) userServer->setInfo(*consinfo);
    //   return reply;
}
