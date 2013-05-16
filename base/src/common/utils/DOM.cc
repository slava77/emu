// $Id: DOM.cc,v 1.6 2012/06/22 21:04:10 banicz Exp $

#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/utils/String.h"
#include "emu/utils/IO.h"
#include "emu/exception/Exception.h"

#include <exception>
#include <sstream>

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/StdOutFormatTarget.hpp"
#include "xercesc/framework/MemBufFormatTarget.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/SAXParseException.hpp"

#include "xalanc/PlatformSupport/XSLException.hpp"
#include "xalanc/XPath/XPathEvaluator.hpp"
#include "xalanc/XPath/NodeRefList.hpp"
#include "xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XercesParserLiaison/XercesDOMSupport.hpp"
#include "xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp"
#include "xalanc/XercesParserLiaison/XercesParserLiaison.hpp"
#include "xalanc/XercesParserLiaison/XercesDocumentWrapper.hpp"
#include "xalanc/XSLT/XSLTInputSource.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeParserLiaison.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp"
#include "xalanc/XalanSourceTree/XalanSourceTreeInit.hpp"

#include "xoap/domutils.h" // for XMLCh2String


XERCES_CPP_NAMESPACE_USE

std::string emu::utils::serializeDOM( DOMNode* node )
{
  std::string result;

  XMLCh tempStr[100];
  XMLString::transcode("LS", tempStr, 99);
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
  DOMWriter* theSerializer = ((DOMImplementationLS*) impl)->createDOMWriter();

  try
  {
    // optionally you can set some features on this serializer
    if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
      theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

    if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
      theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

    // Try to influence what the XML declaration will say:
    // Set encoding explicitly to UTF-8 (otherwise it'll be UTF-16).
    XMLCh* encoding = XMLString::transcode( "UTF-8" );
    theSerializer->setEncoding( encoding );
    XMLString::release( &encoding );

    // cout << "DOM encoding: " << xoap::XMLCh2String( node->getOwnerDocument()->getEncoding() ) << endl;
    // cout << "DOM encoding: " << xoap::XMLCh2String( (static_cast<DOMDocument*>(node))->getEncoding() ) << endl;
    // cout << "Serializer encoding: " << xoap::XMLCh2String( theSerializer->getEncoding() ) << endl;

    MemBufFormatTarget *myFormTarget;
    myFormTarget = new MemBufFormatTarget();

    //
    // do the serialization through DOMWriter::writeNode();
    //
    theSerializer->writeNode(myFormTarget, *node);

    result.append( (const char*) myFormTarget->getRawBuffer(), (std::string::size_type) myFormTarget->getLen() );

    theSerializer->release();

    //
    // Filter, formatTarget and error handler are NOT owned by the serializer.
    //
    delete myFormTarget;
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to serialize DOM: ", e);
  }
  catch (const XMLException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    std::stringstream ss;
    ss << "Failed to serialize DOM: " << message;
    XMLString::release(&message);
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (const DOMException& toCatch)
  {
    char* message = XMLString::transcode(toCatch.getMessage());
    std::stringstream ss;
    ss << "Failed to serialize DOM: " << message;
    XMLString::release(&message);
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize DOM: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to serialize DOM: Unexpected exception.");
  }

  return result;
}


void emu::utils::setNodeValue( DOMNode* node, const std::string& value )
{
  if (node == NULL) return;

  XMLCh* newValue = XMLString::transcode(value.c_str());

  if (node->getNodeType() == DOMNode::ELEMENT_NODE)
  {
    node->setTextContent(newValue);
  }
  else if (node->getNodeType() == DOMNode::ATTRIBUTE_NODE)
  {
    node->setNodeValue(newValue);
  }

  XMLString::release(&newValue);
}


std::string emu::utils::appendToSelectedNode( const std::string &XML,
                                              const std::string &xPathToNode,
                                              const std::string &xmlFragment )
{
  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  std::string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)
  XALAN_USING_XERCES(SAXParseException)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try
  {
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "appendToSelectedNode";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
    XalanDocument* xalan_document = theLiaison.parseXMLStream(theInputSource);
    // cout << "Original XML from Xalan" << endl << emu::utils::serialize( xalan_document ) << endl;
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver(docWrapper);
    
    XPathEvaluator theEvaluator;
    
    XalanNode* xalan_node = theEvaluator.selectSingleNode(theDOMSupport, xalan_document,
                                                          XalanDOMString(xPathToNode.c_str()).c_str(),
                                                          thePrefixResolver);
    if (xalan_node)
    {
      // XalanDOMString nodeName = xalan_node->getNodeName();
      // std::cout << "Found node " << nodeName << std::endl;
      
      DOMNode* node = const_cast< DOMNode* >(docWrapper->mapNode(xalan_node) );
      if ( node )
      {
        // cout << "---------" << endl;
        // cout << "   node->getNodeName()  " << xoap::XMLCh2String( node->getNodeName() )  << endl;
        // cout << "   node->getNodeValue() " << xoap::XMLCh2String( node->getNodeValue() ) << endl;
        // cout << "   node->getNodeType()  " << node->getNodeType()   << endl;

        DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );

        // This method does output UTF-8, it's just the XML declaration that will claim it's UTF-16.
        // The problem is that emacs is too smart, and is mislead by it. It must be forced to open it as UTF-8 by C-x C-m c utf-8 RET.
        // Try to influence what the XML declaration will say:

        // cout << "Encoding: " << xoap::XMLCh2String( domDoc->getEncoding() )
        //      << " Actual encoding: " << xoap::XMLCh2String( domDoc->getActualEncoding() ) << endl;
        // // Set encoding explicitly to UTF-8 (otherwise it'll be UTF-16). It doesn't seem to work...
        // XMLCh* encoding = XMLString::transcode( "UTF-8" );
        // domDoc->setEncoding( encoding );
        // XMLString::release( &encoding );
        // cout << "Encoding: " << xoap::XMLCh2String( domDoc->getEncoding() )
        //      << " Actual encoding: " << xoap::XMLCh2String( domDoc->getActualEncoding() ) << endl;
        // cout << "Original XML from DOM" << endl << emu::utils::serializeDOM( domDoc ) << endl;

        // Parse the XML fragment into a DOM
        MemBufInputSource xmlFragmentMBIS( (XMLByte*)xmlFragment.c_str(), xmlFragment.size(), "appendToSelectedNodeId" );
        XercesDOMParser parser;
        parser.parse( xmlFragmentMBIS );
        DOMElement *xmlFragmentDOM = parser.getDocument()->getDocumentElement();

        // Import and append XML fragment
        DOMNode *importedNode = domDoc->importNode( xmlFragmentDOM , true );
        node->appendChild( importedNode );

        modifiedXML = emu::utils::serializeDOM( domDoc );
        // cout << "Modified XML from DOM" << endl << modifiedXML << endl;
      }
    }

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (SAXParseException& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to append to selected node: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to append to selected node: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to append to selected node: Unknown exception.");
  }
  
  return modifiedXML;
}


std::string emu::utils::setSelectedNodesValues( const std::string &XML,
						const std::string &xPathToNode,
						const std::string &value,
						Operation_t operation )
{
  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  std::string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(NodeRefList);
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)
  XALAN_USING_XERCES(SAXParseException)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try
  {
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "setSelectedNodeValue";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
    XalanDocument* xalan_document = theLiaison.parseXMLStream(theInputSource);
    
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver(docWrapper);
    
    XPathEvaluator theEvaluator;
    
    NodeRefList nodes;
    nodes = theEvaluator.selectNodeList(nodes, theDOMSupport, xalan_document, 
					XalanDOMString(xPathToNode.c_str()).c_str(), 
					thePrefixResolver);

    for (XalanDOMString::size_type i = 0; i < nodes.getLength(); ++i)
    {
      DOMNode* node = const_cast< DOMNode* >(docWrapper->mapNode( nodes.item(i) ) );
      if ( node )
      {
	if ( operation == emu::utils::replace ){
	  emu::utils::setNodeValue( node, value );
	  }
	else if ( operation == emu::utils::add ){
	  std::string oldValue = emu::utils::getNodeValue( node );
	  if ( value.find_first_of(".eE") != std::string::npos || oldValue.find_first_of(".eE") != std::string::npos ){
	    emu::utils::setNodeValue( node, emu::utils::stringFrom<double>( emu::utils::stringTo<double>( oldValue ) + emu::utils::stringTo<double>( value ) ) );
	  }
	  else{
	    emu::utils::setNodeValue( node, emu::utils::stringFrom<int>( emu::utils::stringTo<int>( oldValue ) + emu::utils::stringTo<int>( value ) ) );
	  }
	}
	else if ( operation == emu::utils::prepend ){
	  emu::utils::setNodeValue( node, value + emu::utils::getNodeValue( node ) );
	}
	else if ( operation == emu::utils::append ){
	  emu::utils::setNodeValue( node, emu::utils::getNodeValue( node ) + value );
	}
	else{
	  std::stringstream ss;
	  ss << "Unknown operation (" << operation << ") on node value.";
	  XCEPT_RAISE( xcept::Exception, ss.str() );
	}
      }
    }

    DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );
    modifiedXML = emu::utils::serializeDOM( domDoc );

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (SAXParseException& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to set node value: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to set node value: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to set node value: Unknown exception.");
  }
  
  return modifiedXML;
}


std::string emu::utils::setSelectedNodesValues(const std::string &XML,
                                               const std::map<std::string, std::string> &values,
					       Operation_t operation)
{
  // Based on the idea in http://www.opensubscriber.com/message/xalan-c-users@xml.apache.org/2655850.html

  std::string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(NodeRefList);
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)
  XALAN_USING_XERCES(SAXParseException)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try
  {
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "setSelectedNodesValues";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
    XalanDocument* xalan_document = theLiaison.parseXMLStream(theInputSource);
    
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver(docWrapper);
    
    XPathEvaluator theEvaluator;

    std::map< std::string, std::string >::const_iterator v;
    for (v = values.begin(); v != values.end(); ++v)
    {
      NodeRefList nodes;
      nodes = theEvaluator.selectNodeList(nodes, theDOMSupport, xalan_document, 
					  XalanDOMString(v->first.c_str()).c_str(), 
					  thePrefixResolver);

      for (XalanDOMString::size_type i = 0; i < nodes.getLength(); ++i)
      {
	DOMNode* node = const_cast< DOMNode* >(docWrapper->mapNode( nodes.item(i) ) );
	if ( node )
	{
	  if ( operation == emu::utils::replace ){
	    emu::utils::setNodeValue( node, v->second );
	  }
	  else if ( operation == emu::utils::add ){
	    std::string oldValue = emu::utils::getNodeValue( node );
	    if ( v->second.find_first_of(".eE") != std::string::npos || oldValue.find_first_of(".eE") != std::string::npos ){
	      emu::utils::setNodeValue( node, emu::utils::stringFrom<double>( emu::utils::stringTo<double>( oldValue ) + emu::utils::stringTo<double>( v->second ) ) );
	    }
	    else{
	      emu::utils::setNodeValue( node, emu::utils::stringFrom<int>( emu::utils::stringTo<int>( oldValue ) + emu::utils::stringTo<int>( v->second ) ) );
	    }
	  }
	  else if ( operation == emu::utils::prepend ){
	    emu::utils::setNodeValue( node, v->second + emu::utils::getNodeValue( node ) );
	  }
	  else if ( operation == emu::utils::append ){
	    emu::utils::setNodeValue( node, emu::utils::getNodeValue( node ) + v->second );
	  }
	  else{
	    std::stringstream ss;
	    ss << "Unknown operation (" << operation << ") on node value.";
	    XCEPT_RAISE( xcept::Exception, ss.str() );
	  }
	}
      }
      
    } // for ( v = values.begin(); v != values.end(); ++v )

    DOMDocument *domDoc = const_cast< DOMDocument* >(docWrapper->getXercesDocument() );
    modifiedXML = emu::utils::serializeDOM(domDoc);
    // cout << "modifiedXML" << endl << modifiedXML << endl;

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (SAXParseException& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to set nodes' values: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to set nodes' values: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to set nodes' values: Unknown exception.");
  }
  
  return modifiedXML;
}


std::string emu::utils::getSelectedNodeValue( const std::string &XML,
                                              const std::string &xpath )
{
  std::string value;

  XALAN_USING_XALAN(XSLException);
  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XalanDOMString)

  try
  {
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)

    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();

    XALAN_USING_XALAN(XalanDocument)
    XALAN_USING_XALAN(XalanDocumentPrefixResolver)
    XALAN_USING_XALAN(XalanNode)
    XALAN_USING_XALAN(XalanSourceTreeInit)
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport)
    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XALAN_USING_XALAN(XObjectPtr)

    // Initialize the XalanSourceTree subsystem...
    XalanSourceTreeInit theSourceTreeInit;

    // We'll use these to parse the XML file.
    XalanSourceTreeDOMSupport theDOMSupport;
    XalanSourceTreeParserLiaison theLiaison(theDOMSupport);

    // Hook the two together...
    theDOMSupport.setParserLiaison(&theLiaison);

    const char* const id = "getSelectedNodeValue";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);

    // Parse the document...
    XalanDocument* const theDocument = theLiaison.parseXMLStream(theInputSource);

    XalanDocumentPrefixResolver thePrefixResolver(theDocument);

    XPathEvaluator theEvaluator;

    // OK, let's find the node...
    XalanNode* const node = theEvaluator.selectSingleNode(theDOMSupport, theDocument,
                                                          XalanDOMString(xpath.c_str()).c_str(), thePrefixResolver);
    value = emu::utils::getNodeValue(node);

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to get node value: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to get node value: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to get node value: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    std::stringstream ss;
    ss << "Failed to get node value: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to get node value: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to get node value: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to get node value: Unknown exception.");
  }

  return value;
}


std::vector< std::pair<std::string, std::string> > emu::utils::getSelectedNodesValues( const std::string &XML,
                                                                                       const std::string &xpath )
{
  std::vector< std::pair<std::string, std::string> > nameValuePairs;

  // XALAN_USING_XALAN(XalanNode);
  // XalanNode* theNode;

  XALAN_USING_XALAN(XSLException);
  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XalanDOMString);

  try
  {
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();

    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // Initialize the XalanSourceTree subsystem...
    XALAN_USING_XALAN(XalanSourceTreeInit);
    XalanSourceTreeInit          theSourceTreeInit;

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport    theDOMSupport;

    XALAN_USING_XALAN(XalanSourceTreeParserLiaison)
    XalanSourceTreeParserLiaison theLiaison( theDOMSupport );
    theDOMSupport.setParserLiaison(&theLiaison);

    const char* const id = "getSelectedNodesValues";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
    XALAN_USING_XALAN(XalanDocument)
    XalanDocument* document = theLiaison.parseXMLStream(theInputSource);

    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver thePrefixResolver(document);

    XPathEvaluator theEvaluator;

    XALAN_USING_XALAN(NodeRefList);

    XalanDOMString xpathXalan(xpath.c_str());

    NodeRefList nodes;
    nodes = theEvaluator.selectNodeList(nodes, theDOMSupport, document, xpathXalan.data(), thePrefixResolver);

    for (XalanDOMString::size_type i = 0; i < nodes.getLength(); ++i)
    {
      nameValuePairs.push_back(
          std::make_pair(emu::utils::stringFrom(nodes.item(i)->getNodeName()),
                         emu::utils::getNodeValue(nodes.item(i))));
    }

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to get nodes' values: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to get nodes' values: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to get nodes' values: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    std::stringstream ss;
    ss << "Failed to get nodes' values: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to get nodes' values: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to get nodes' values: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to get nodes' values: Unknown exception.");
  }

  return nameValuePairs;
}


std::string emu::utils::getSelectedNode(const std::string &XML,
                                        const std::string &xpath)
{
  std::string nodeXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)
  XALAN_USING_XERCES(SAXParseException)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try
  {
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();

    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);

    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "setSelectedNodesValues";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );

    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);

    XalanDocumentPrefixResolver thePrefixResolver( docWrapper );

    XPathEvaluator theEvaluator;

    XalanNode* xalan_node = theEvaluator.selectSingleNode( theDOMSupport,
                                                           xalan_document,
                                                           XalanDOMString( xpath.c_str() ).c_str(),
                                                           thePrefixResolver );
    if ( xalan_node )
    {
      DOMNode* node = const_cast<DOMNode*>( docWrapper->mapNode( xalan_node ) );
      if ( node )
      {
         nodeXML = serializeDOM( node );
      }
    }

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch( SAXParseException& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch( XMLException& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch( DOMException& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch( XalanDOMException& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch( XSLException& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch( xcept::Exception& e )
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to set nodes' values: ", e );
  }
  catch( std::exception& e )
  {
    std::stringstream ss; ss << "Failed to set nodes' values: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str() );
  }
  catch(...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to set nodes' values: Unknown exception." );
  }

  return nodeXML;
}


std::string emu::utils::getNodeValue( const DOMNode* const node )
{
  std::string value;
  if (node)
  {
    if (node->getNodeType() == DOMNode::ELEMENT_NODE && node->getFirstChild())
    {
      if (node->getFirstChild()->getNodeType() == DOMNode::TEXT_NODE)
      {
        value = xoap::XMLCh2String( node->getFirstChild()->getNodeValue() );
      }
    }
    else
    {
      value = xoap::XMLCh2String( node->getNodeValue() );
    }
  }
  return value;
}

std::string emu::utils::getNodeValue( const XalanNode* const node )
{
  std::stringstream value;
  XALAN_USING_XALAN(XalanNode)
  if (node)
  {
    if (node->getNodeType() == XalanNode::ELEMENT_NODE && node->getFirstChild())
    {
      if (node->getFirstChild()->getNodeType() == XalanNode::TEXT_NODE)
      {
        value << node->getFirstChild()->getNodeValue();
      }
    }
    else
    {
      value << node->getNodeValue();
    }
  }
  return value.str();
}


std::string emu::utils::removeSelectedNode( const std::string& XML, 
					    const std::string& xpath )
{

  std::string modifiedXML;

  XALAN_USING_XALAN(XalanDOMString)
  XALAN_USING_XALAN(XalanDocument)
  XALAN_USING_XALAN(XalanDocumentPrefixResolver)
  XALAN_USING_XALAN(XalanNode)
  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesDOMWrapperParsedSource)
  XALAN_USING_XALAN(XercesDocumentWrapper)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XALAN(XalanElement)
  XALAN_USING_XERCES(DOMNode)
  XALAN_USING_XERCES(DOMAttr)
  XALAN_USING_XERCES(SAXParseException)

  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  XALAN_USING_XERCES(XMLPlatformUtils)
  XALAN_USING_XALAN(XPathEvaluator)

  try
  {
    // Namespaces won't work if these are not initialized:
    XMLPlatformUtils::Initialize();
    XPathEvaluator::initialize();
    
    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    // Create an input source that represents a local file...
    // const XalanDOMString    theFileName(XML.c_str());
    // const LocalFileInputSource      theInputSource(theFileName.c_str());
    const char* const id = "removeSelectedNode";
    MemBufInputSource theInputSource( (const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id );
    XalanDocument* xalan_document = theLiaison.parseXMLStream( theInputSource );
    // cout << "Original XML from Xalan" << endl << emu::utils::serialize( xalan_document ) << endl;
    XercesDocumentWrapper* docWrapper = theLiaison.mapDocumentToWrapper(xalan_document);
    
    XalanDocumentPrefixResolver thePrefixResolver( docWrapper );
    
    XPathEvaluator theEvaluator;
    
    XALAN_USING_XALAN(NodeRefList);

    XalanDOMString xpathXalan( xpath.c_str() );

    NodeRefList nodes;
    nodes = theEvaluator.selectNodeList( nodes,
    					 theDOMSupport,
    					 xalan_document,
    					 xpathXalan.data(),
    					 thePrefixResolver );

    // Convert Xalan nodes into DOM nodes
    std::vector<DOMNode*> DOMNodes;
    for ( XalanDOMString::size_type i=0; i<nodes.getLength(); ++i ){
      DOMNode* node = const_cast<DOMNode*>( docWrapper->mapNode( nodes.item(i) ) );
      if ( node ) DOMNodes.push_back( node );
    }
    
    // Remove DOM nodes
    DOMDocument *domDoc = const_cast<DOMDocument*>( docWrapper->getXercesDocument() );
    for ( std::vector<DOMNode*>::iterator dn = DOMNodes.begin(); dn != DOMNodes.end(); ++dn ){
      switch ( (*dn)->getNodeType() ){
      case DOMNode::ELEMENT_NODE:
      case DOMNode::TEXT_NODE:
	(*dn)->getParentNode()->removeChild( *dn );
	break;
      case DOMNode::ATTRIBUTE_NODE:
	DOMAttr *attr = dynamic_cast<DOMAttr*>(*dn);
	attr->getOwnerElement()->removeAttributeNode( attr );
	break;
      }
    }

    modifiedXML = emu::utils::serializeDOM( domDoc );
    // cout << "Modified XML from DOM" << endl << modifiedXML << endl;

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser 
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch( SAXParseException& e )
  {
    std::stringstream ss;
    ss << "Failed to remove selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XMLException& e )
  {
    std::stringstream ss;
    ss << "Failed to remove selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( DOMException& e )
  {
    std::stringstream ss;
    ss << "Failed to remove selected node: " << xoap::XMLCh2String( e.getMessage() );
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XalanDOMException& e )
  {
    std::stringstream ss;
    ss << "Failed to remove selected node: exception code " << e.getExceptionCode();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( XSLException& e )
  {
    std::stringstream ss;
    ss << "Failed to remove selected node: XSLException type: " << XalanDOMString( e.getType() ) << ", message: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( xcept::Exception& e )
  {
    XCEPT_RETHROW( xcept::Exception, "Failed to remove selected node: ", e );
  }
  catch( std::exception& e )
  {
      std::stringstream ss; 
      ss << "Failed to remove selected node: " << e.what();
      XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...)
  {
    XCEPT_RAISE( xcept::Exception, "Failed to remove selected node: Unknown exception." );
  }
  
  return modifiedXML;
}
