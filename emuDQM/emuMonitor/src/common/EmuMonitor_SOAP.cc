/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.                                        *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE.                                  *
 * For the list of contributors see CREDITS.                             *
 *************************************************************************/


#include "EmuMonitor.h"

xoap::MessageReference EmuMonitor::requestObjectsList(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Received Monitoring Objects List request");
  xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
  if (rb.hasFault() )
    {
      xoap::SOAPFault fault = rb.getFault();
      std::string errmsg = "DQMNode: ";
      errmsg += fault.getFaultString();
      XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } 

  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjectsList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPElement command = body.addBodyElement(commandName );
  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));

  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());

  xoap::SOAPName histoName = envelope.createName("Obj", "", "");
  xoap::SOAPName histodirName = envelope.createName("Branch", "", "");

  if (plotter_ != NULL) {
	
    int tout=0;
    while ((plotter_->isBusy()) && (tout <sTimeout*2)) {
      usleep(500000); tout++;
    };
    if (tout==sTimeout*2) {
      LOG4CPLUS_WARN (getApplicationLogger(), "Plotter is still busy after " << sTimeout << " secs");
      return msg;
    }


    map<string, ME_List> MEs = plotter_->GetMEs();

    for (map<string, ME_List >::iterator itr = MEs.begin();
	 itr != MEs.end(); ++itr) {
      xdata::String dir(itr->first);
      xoap::SOAPElement histodirElement = monitorElement.addChildElement(histodirName);
      histodirElement.addTextNode(dir);
      for (ME_List_const_iterator h_itr = itr->second.begin();
	   h_itr != itr->second.end(); ++h_itr) {
	xdata::String hname  (h_itr->first);
	xoap::SOAPElement histoElement = histodirElement.addChildElement(histoName);
	histoElement.addTextNode(hname);

	//	LOG4CPLUS_INFO(getApplicationLogger(),
	//	       "ME: " << itr->first << "/" << h_itr->second->getFullName() << " size: " << sizeof(*(h_itr->second->getObject())));

      }
    }
  }
  return msg;
}

xoap::MessageReference EmuMonitor::requestObjects(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Received Monitoring Objects request");
  xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
  std::string folder = "";
  std::string objname = "";
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestObjects","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPElement command = body.addBodyElement(commandName );
  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));

  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());

  if (rb.hasFault() )
    {
      xoap::SOAPFault fault = rb.getFault();
      std::string errmsg = "DQMNode: ";
      errmsg += fault.getFaultString();
      XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } else {
      if (plotter_ != NULL) 
	{
	  int tout=0;
          while ((plotter_->isBusy()) && (tout <sTimeout*2)) {
	    usleep(500000); tout++;
          };
          if (tout==sTimeout*2) { 
	    LOG4CPLUS_WARN (getApplicationLogger(), "Plotter is still busy after " << sTimeout << " secs");
	    return msg;
	  }

	  map<string, ME_List > MEs = plotter_->GetMEs();
          xoap::SOAPName cmdTag = envelope.createName("requestObjects","xdaq", "urn:xdaq-soap:3.0");
	  vector<xoap::SOAPElement> content = rb.getChildElements (cmdTag);
	  for (std::vector<xoap::SOAPElement>::iterator n_itr = content.begin();
	       n_itr != content.end(); ++n_itr)
	    {
	      xoap::SOAPName folderTag ("Folder", "", "");
	      std::vector<xoap::SOAPElement> folderElement = n_itr->getChildElements (folderTag );
	      for (std::vector<xoap::SOAPElement>::iterator f_itr = folderElement.begin();
		   f_itr != folderElement.end(); ++f_itr) 
		{
		  folder = f_itr->getValue();
		  // == Find object folder in MEs list
		  map<string, ME_List >::iterator melist_itr = MEs.find(folder);
		  if (melist_itr != MEs.end()) 
		    {	   	      
		      xoap::SOAPName objectTag ("Object", "", "");
		      std::vector<xoap::SOAPElement> objectElement = f_itr->getChildElements (objectTag );
		      for (std::vector<xoap::SOAPElement>::iterator o_itr = objectElement.begin();
			   o_itr != objectElement.end(); ++o_itr) 
			{
			  objname = o_itr->getValue();
			  // == Find object in MEs list
			  ME_List_const_iterator meobj_itr = melist_itr->second.find(objname);
			  if (meobj_itr != melist_itr->second.end()) {
			    TMessage buf(kMESS_OBJECT);
			    buf.Reset();
			    buf.SetWriteMode();
			    buf.WriteObjectAny(meobj_itr->second->getObject(), meobj_itr->second->getObject()->Class());
			    char * attch_buf = new char[buf.BufferSize()];
			    buf.Reset();
			    buf.SetReadMode();
			    buf.ReadBuf(attch_buf, buf.BufferSize());
			    std::string contenttype = "application/octet-stream";
			    xoap::AttachmentPart * attachment = msg->createAttachmentPart(attch_buf, buf.BufferSize(), contenttype);
			    //attachment->addMimeHeader("Content-Description", h_itr->first);
			    attachment->setContentLocation(folder+"/"+objname);
			    attachment->setContentEncoding("binary");
			    msg->addAttachmentPart(attachment);
			    LOG4CPLUS_INFO (getApplicationLogger(), "Sending "<<  meobj_itr->second->getFullName());
			    delete []attch_buf;
			  }
			}
		    }
		}
	    }
	}
    }

  return msg;
}

xoap::MessageReference EmuMonitor::requestCanvasesList(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Received Monitoring Canvases List request");
  xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
  if (rb.hasFault() )
    {
      xoap::SOAPFault fault = rb.getFault();
      std::string errmsg = "DQMNode: ";
      errmsg += fault.getFaultString();
      XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } 

  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvasesList","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPElement command = body.addBodyElement(commandName );
  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));

  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());

  xoap::SOAPName histoName = envelope.createName("Obj", "", "");
  xoap::SOAPName histodirName = envelope.createName("Branch", "", "");

  if (plotter_ != NULL) {
    int tout=0;
    while ((plotter_->isBusy()) && (tout <sTimeout*2)) {
      usleep(500000); tout++;
    };
    if (tout==sTimeout*2) {
      LOG4CPLUS_WARN (getApplicationLogger(), "Plotter is still busy after " << sTimeout << " secs");
      return msg;
    }

    map<string, MECanvases_List> MECanvases = plotter_->GetMECanvases();

    for (map<string, MECanvases_List >::iterator itr = MECanvases.begin();
	 itr != MECanvases.end(); ++itr) {
      xdata::String dir(itr->first);
      xoap::SOAPElement histodirElement = monitorElement.addChildElement(histodirName);
      histodirElement.addTextNode(dir);
      for (MECanvases_List_const_iterator h_itr = itr->second.begin();
	   h_itr != itr->second.end(); ++h_itr) {
	xdata::String hname  (h_itr->first);
	xoap::SOAPElement histoElement = histodirElement.addChildElement(histoName);
	histoElement.addTextNode(hname);
	
	//	LOG4CPLUS_INFO(getApplicationLogger(),
	//       "Canvas: " << itr->first << "/" << h_itr->second->getFullName());
	
      }
    }
  }
  return msg;
}


xoap::MessageReference EmuMonitor::requestCanvas(xoap::MessageReference node) throw (xoap::exception::Exception)
{
  LOG4CPLUS_INFO (getApplicationLogger(), "Received Monitoring Canvas request");
  xoap::SOAPBody rb = node->getSOAPPart().getEnvelope().getBody();
  std::string folder = "";
  std::string objname = "";
  xoap::MessageReference msg = xoap::createMessage();
  xoap::SOAPEnvelope envelope = msg->getSOAPPart().getEnvelope();
  xoap::SOAPBody body = envelope.getBody();
  xoap::SOAPName commandName = envelope.createName("requestCanvas","xdaq", "urn:xdaq-soap:3.0");
  xoap::SOAPElement command = body.addBodyElement(commandName );
  //  xoap::SOAPName statusName = envelope.createName("Status", "", "");
  //  xoap::SOAPElement statusElement = command.addChildElement(statusName);
  xdata::Integer localTid(i2o::utils::getAddressMap()->getTid(this->getApplicationDescriptor()));
  xoap::SOAPName monitorName = envelope.createName("DQMNode", "", "");
  xoap::SOAPElement monitorElement = command.addChildElement(monitorName);
  monitorElement.addTextNode(localTid.toString());

  if (rb.hasFault() )
    {
      xoap::SOAPFault fault = rb.getFault();
      std::string errmsg = "DQMNode: ";
      errmsg += fault.getFaultString();
      XCEPT_RAISE(xoap::exception::Exception, errmsg);
    } else {
      if (plotter_ != NULL) 
	{
	  int tout=0;
          while ((plotter_->isBusy()) && (tout <sTimeout*2)) {
	    usleep(500000); tout++; 
	  };
	  if (tout==sTimeout*2) { 
	    LOG4CPLUS_WARN (getApplicationLogger(), "Plotter is still busy after " << sTimeout << " secs");
	    return msg;
	  }
	  map<string, ME_List > MEs = plotter_->GetMEs();
	  map<string, MECanvases_List > MECanvases = plotter_->GetMECanvases();
          xoap::SOAPName cmdTag = envelope.createName("requestCanvas","xdaq", "urn:xdaq-soap:3.0");
	  vector<xoap::SOAPElement> content = rb.getChildElements (cmdTag);
	  for (std::vector<xoap::SOAPElement>::iterator n_itr = content.begin();
	       n_itr != content.end(); ++n_itr)
	    {
	      xoap::SOAPName folderTag ("Folder", "", "");
	      std::vector<xoap::SOAPElement> folderElement = n_itr->getChildElements (folderTag );
	      for (std::vector<xoap::SOAPElement>::iterator f_itr = folderElement.begin();
		   f_itr != folderElement.end(); ++f_itr) 
		{
		  folder = f_itr->getValue();
		  // == Find object folder in MEs list
		  map<string, ME_List >::iterator melist_itr = MEs.find(folder);
		  map<string, MECanvases_List >::iterator cnvlist_itr = MECanvases.find(folder);
		  // if (melist_itr != MEs.end()) 
		  if ((cnvlist_itr != MECanvases.end()) && (melist_itr != MEs.end()))
		    {	   	      		      
		      xoap::SOAPName objectTag ("Canvas", "", "");
		      std::vector<xoap::SOAPElement> objectElement = f_itr->getChildElements (objectTag );
		      for (std::vector<xoap::SOAPElement>::iterator o_itr = objectElement.begin();
			   o_itr != objectElement.end(); ++o_itr) 
			{
			  objname = o_itr->getValue();

			  // == Find object in MEs list
			  // ME_List_const_iterator meobj_itr = melist_itr->second.find(objname);
			  MECanvases_List_const_iterator cnvobj_itr = cnvlist_itr->second.find(objname);
			  if (cnvobj_itr != cnvlist_itr->second.end()) {
			    TMessage buf(kMESS_OBJECT);
			    buf.Reset();
			    buf.SetWriteMode();
			    EmuMonitoringCanvas* cnv = new EmuMonitoringCanvas(*(cnvobj_itr->second));
			    
			    xoap::SOAPName widthTag ("Width", "", "");
			    if (o_itr->getAttributeValue(widthTag ) != "")
			      cnv->setCanvasWidth(atoi((o_itr->getAttributeValue (widthTag )).c_str()));
			  
			    xoap::SOAPName heightTag ("Height", "", "");
			    if (o_itr->getAttributeValue (heightTag ) != "")
			      cnv->setCanvasHeight(atoi((o_itr->getAttributeValue (heightTag )).c_str()));

			    cnv->Draw(melist_itr->second);
			    // buf.WriteObjectAny(meobj_itr->second->getObject(), meobj_itr->second->getObject()->Class());
			    buf.WriteObjectAny(cnv->getCanvasObject(), cnv->getCanvasObject()->Class());
			    char * attch_buf = new char[buf.BufferSize()];
			    buf.Reset();
			    buf.SetReadMode();
			    buf.ReadBuf(attch_buf, buf.BufferSize());
			    std::string contenttype = "application/octet-stream";
			    xoap::AttachmentPart * attachment = msg->createAttachmentPart(attch_buf, buf.BufferSize(), contenttype);
			    //attachment->addMimeHeader("Content-Description", h_itr->first);
			    attachment->setContentLocation(folder+"/"+objname);
			    attachment->setContentEncoding("binary");
			    msg->addAttachmentPart(attachment);
			    LOG4CPLUS_INFO (getApplicationLogger(), "Sending "<<  cnv->getFullName());
			    delete []attch_buf;
			    delete cnv;
			  }
			}
		    }
		}
	    }
	}
    }

  return msg;
}
