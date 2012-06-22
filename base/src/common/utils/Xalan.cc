// $Id: Xalan.cc,v 1.3 2012/06/22 21:04:10 banicz Exp $

#include "emu/utils/Xalan.h"
#include "emu/exception/Exception.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/PlatformSupport/XSLException.hpp>
#include <xalanc/PlatformSupport/XalanStdOutputStream.hpp>
#include <xalanc/PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <xalanc/XMLSupport/FormatterToXML.hpp>
#include <xalanc/XMLSupport/FormatterTreeWalker.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>
#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>

#include "xoap/domutils.h" // for XMLCh2String

#include <sstream>
#include <exception>


int emu::utils::transformStreams(std::istream& source,
                                 std::istream& stylesheet,
                                 std::ostream& target)
{
  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try
  {
    XALAN_USING_XALAN(XalanTransformer);    
    // Initialize Xalan.
    XalanTransformer::initialize();
    // Create a XalanTransformer.
    XalanTransformer theXalanTransformer;

    XALAN_USING_XALAN(XSLTInputSource);
    XALAN_USING_XALAN(XSLTResultTarget);
    XSLTInputSource theSource( source );
    XSLTInputSource theStylesheet( stylesheet );
    XSLTResultTarget theTarget( target );

    // Do the transform.
    result = theXalanTransformer.transform(theSource, theStylesheet, theTarget);
    
    if (result != 0)
    {
      std::stringstream ss;
      ss << "Error: couldn't transform " << theXalanTransformer.getLastError();
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }

    // Do not terminate. Other threads may still use it.
    // // Terminate Xalan...
    // XalanTransformer::terminate();
    // // Clean up the ICU, if it's integrated...
    // XalanTransformer::ICUCleanUp();
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "XSLT transformation failed: ", e);
  }
  catch (XSLException& e)
  {
    XALAN_USING_XALAN(XalanDOMString)
    std::stringstream ss;
    ss << "XSLT transformation failed: XSLException type: " << XalanDOMString(e.getType()) << ", message: "
        << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "XSLT transformation failed: Unknown exception.");
  }
  return result;
}


int emu::utils::transformWithParams( std::istream& source,
                                     std::istream& stylesheet,
                                     std::ostream& target,
                                     const std::map< std::string, std::string >& params)
{
  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try
  {
    XALAN_USING_XALAN(XalanTransformer);    
    // Initialize Xalan.
    XalanTransformer::initialize();
    // Create a XalanTransformer.
    XalanTransformer theXalanTransformer;

    XALAN_USING_XALAN(XSLTInputSource);
    XALAN_USING_XALAN(XSLTResultTarget);
    XSLTInputSource theSource( source );
    XSLTInputSource theStylesheet( stylesheet );
    XSLTResultTarget theTarget( target );

    // Set the stylesheet parameters
    XALAN_USING_XALAN(XalanDOMString);
    for (std::map< std::string, std::string >::const_iterator p = params.begin(); p != params.end(); ++p)
    {
      XalanDOMString name(p->first.c_str());
      XalanDOMString value(p->second.c_str());
      theXalanTransformer.setStylesheetParam(name, value);
    }
    // Do the transform.
    result = theXalanTransformer.transform(theSource, theStylesheet, theTarget);
    
    if (result != 0)
    {
      std::stringstream ss;
      ss << "Error: couldn't transform " << theXalanTransformer.getLastError();
      XCEPT_RAISE( emu::exception::XMLException, ss.str());
    }

    // Do not terminate. Other threads may still use it.
    // // Terminate Xalan...
    // XalanTransformer::terminate();
    // // Clean up the ICU, if it's integrated...
    // XalanTransformer::ICUCleanUp();
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "XSLT transformation failed: ", e);
  }
  catch (XSLException& e)
  {
    XALAN_USING_XALAN(XalanDOMString)
    std::stringstream ss;
    ss << "XSLT transformation failed: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "XSLT transformation failed: Unknown exception.");
  }
  return result;
}


XALAN_USING_XALAN(XalanDocument)


std::string emu::utils::serialize( const XalanNode* node )
{
  XALAN_USING_XALAN(XalanStdOutputStream);
  XALAN_USING_XALAN(XalanOutputStreamPrintWriter);
  XALAN_USING_XALAN(FormatterToXML);
  XALAN_USING_XALAN(FormatterTreeWalker);

  std::stringstream target_stream; // this is the output stream
  XalanStdOutputStream output_stream(target_stream);
  XalanOutputStreamPrintWriter writer(output_stream);
  FormatterToXML formatter(writer);
  FormatterTreeWalker tree_walker(formatter);
  tree_walker.traverse( node );
  return target_stream.str();
}


XALAN_USING_XALAN(XalanNode)


XalanNode* emu::utils::getSingleNode( XalanDocument* document,
                                      XalanNode* contextNode,
                                      const std::string &xPath )
{
  XALAN_USING_XALAN(XalanNode);
  XalanNode* theNode;

  XALAN_USING_XALAN(XSLException);

  try
  {
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver thePrefixResolver(document);
    XPathEvaluator theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theNode = theEvaluator.selectSingleNode(theDOMSupport, contextNode,
                                            XalanDOMString(xPath.c_str()).c_str(), thePrefixResolver);
    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (xcept::Exception& e)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': ";
    XCEPT_RETHROW( emu::exception::XMLException, ss.str(), e);
  }
  catch (XSLException& e)
  {
    XALAN_USING_XALAN(XalanDOMString)
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': Unknown exception.";
    XCEPT_RAISE( emu::exception::UndefinedException, ss.str());
  }
  return theNode;
}


XALAN_USING_XALAN(NodeRefList)


void emu::utils::getNodes( NodeRefList& resultNodeList,
                           XalanDocument* document,
                           XalanNode* contextNode,
                           const std::string &xPath )
{
  XALAN_USING_XALAN(XSLException);

  try
  {
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver thePrefixResolver(document);
    XPathEvaluator theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theEvaluator.selectNodeList(resultNodeList, theDOMSupport, contextNode,
                                XalanDOMString(xPath.c_str()).c_str(), thePrefixResolver);

    // Do not terminate. Other threads may still use it.
    // XPathEvaluator::terminate();

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (xcept::Exception& e)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': ";
    XCEPT_RETHROW( emu::exception::XMLException, ss.str(), e);
  }
  catch (XSLException& e)
  {
    XALAN_USING_XALAN(XalanDOMString)
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    std::stringstream ss;
    ss << "XPath selection failed for '" << xPath << "': Unknown exception.";
    XCEPT_RAISE( emu::exception::UndefinedException, ss.str());
  }
}


std::string emu::utils::serializeSelectedNode( const std::string& XML,
                                               const std::string& xPath )
{
  std::string serializedNode;

  XALAN_USING_XALAN(XercesDOMSupport)
  XALAN_USING_XALAN(XercesParserLiaison)
  XALAN_USING_XERCES(MemBufInputSource)
  XALAN_USING_XERCES(XMLException)
  XALAN_USING_XERCES(SAXParseException)
  XALAN_USING_XALAN(XalanDOMException)
  XALAN_USING_XALAN(XSLException)

  try
  {
    XALAN_USING_XERCES(XMLPlatformUtils)
    XALAN_USING_XALAN(XPathEvaluator)
    XMLPlatformUtils::Initialize();

    XercesDOMSupport theDOMSupport;
    XercesParserLiaison theLiaison(theDOMSupport);
    theLiaison.setDoNamespaces(true); // although it seems to be already set...
    theLiaison.setBuildWrapperNodes(true);
    theLiaison.setBuildMaps(true);
    
    const char* const id = "dummy";
    MemBufInputSource theInputSource((const XMLByte*) XML.c_str(), (unsigned int) XML.size(), id);
    XalanDocument* document = theLiaison.parseXMLStream(theInputSource);

    serializedNode = serialize(getSingleNode(document, document, xPath));

    // XMLPlatformUtils::Terminate() causes the program to crash unless XMLPlatformUtils::Initialize()
    // has been called more times than has XMLPlatformUtils::Terminate(). Anyway, as of Xerces-C++ 2.8.0:
    // "The termination call is currently optional, to aid those dynamically loading the parser
    // to clean up before exit, or to avoid spurious reports from leak detectors."
    // XMLPlatformUtils::Terminate();
  }
  catch (SAXParseException& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XMLException& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (DOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize selected node: " << xoap::XMLCh2String(e.getMessage());
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XalanDOMException& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize selected node: exception code " << e.getExceptionCode();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (XSLException& e)
  {
    XALAN_USING_XALAN(XalanDOMString)
    std::stringstream ss;
    ss << "Failed to serialize selected node: XSLException type: " << XalanDOMString(e.getType()) << ", message: " << e.getMessage();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (xcept::Exception& e)
  {
    XCEPT_RETHROW( emu::exception::XMLException, "Failed to serialize selected node: ", e);
  }
  catch (std::exception& e)
  {
    std::stringstream ss;
    ss << "Failed to serialize selected node: " << e.what();
    XCEPT_RAISE( emu::exception::XMLException, ss.str());
  }
  catch (...)
  {
    XCEPT_RAISE( emu::exception::UndefinedException, "Failed to serialize selected node: Unknown exception.");
  }
  
  return serializedNode;
}
