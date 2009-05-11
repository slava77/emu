#include "emu/farmer/utils/Xalan.h"

#include "emu/farmer/EmuXalanTransformerProblemListener.h"
#include <exception>

// headers needed for Xalan transform
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xalanc/PlatformSupport/XalanMemoryManagerDefault.hpp>

#include <xalanc/PlatformSupport/XSLException.hpp>

// headers needed for Xalan serialize
#include <sstream>
#include <xalanc/PlatformSupport/XalanStdOutputStream.hpp>
#include <xalanc/PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <xalanc/XMLSupport/FormatterToXML.hpp>
#include <xalanc/XMLSupport/FormatterTreeWalker.hpp>
#include <xalanc/XPath/XPathEvaluator.hpp>
#include <xalanc/XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <xalanc/DOMSupport/XalanDocumentPrefixResolver.hpp>

using namespace std;

int
emu::farmer::utils::transformStreams( istream& source, istream& stylesheet, ostream& target )
  throw( xcept::Exception ){

  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try{

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
    
    if(result != 0){
      stringstream ss; ss << "Error: " << theXalanTransformer.getLastError();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }

    // Terminate Xalan...
    XalanTransformer::terminate();
    // Clean up the ICU, if it's integrated...
    XalanTransformer::ICUCleanUp();
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "XSLT transformation failed: ", e );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "XSLT transformation failed: Unknown exception." );
  }
  return result;
}

int
emu::farmer::utils::transformWithParams( istream& source, istream& stylesheet, ostream& target, const map<std::string,std::string>& params )
  throw( xcept::Exception ){

  XALAN_USING_XALAN( XSLException );
  int result = 1;
  try{

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
    for ( map<std::string,std::string>::const_iterator p = params.begin(); p != params.end(); ++p ){
      XalanDOMString name( p->first.c_str() );
      XalanDOMString value( p->second.c_str() );
      theXalanTransformer.setStylesheetParam( name, value );
    }
    // Do the transform.
    result = theXalanTransformer.transform(theSource, theStylesheet, theTarget);
    
    if(result != 0){
      stringstream ss; ss << "Error: " << theXalanTransformer.getLastError();
      XCEPT_RAISE( xcept::Exception, ss.str() );
    }

    // Terminate Xalan...
    XalanTransformer::terminate();
    // Clean up the ICU, if it's integrated...
    XalanTransformer::ICUCleanUp();
  }
  catch( xcept::Exception& e ){
    XCEPT_RETHROW( xcept::Exception, "XSLT transformation failed: ", e );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XSLT transformation failed: " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    XCEPT_RAISE( xcept::Exception, "XSLT transformation failed: Unknown exception." );
  }
  return result;
}


XALAN_USING_XALAN(XalanDocument)

string
emu::farmer::utils::serialize( const XalanDocument* document )
  throw( xcept::Exception ){
  XALAN_USING_XALAN(XalanStdOutputStream);
  XALAN_USING_XALAN(XalanOutputStreamPrintWriter);
  XALAN_USING_XALAN(FormatterToXML);
  XALAN_USING_XALAN(FormatterTreeWalker);
  stringstream target_stream; // this is the output stream
  XalanStdOutputStream output_stream(target_stream);
  XalanOutputStreamPrintWriter writer(output_stream);
  FormatterToXML formatter(writer);
  FormatterTreeWalker tree_walker(formatter);
  tree_walker.traverse(document);
  return target_stream.str();
}

XALAN_USING_XALAN(XalanNode)

XalanNode*
emu::farmer::utils::getSingleNode( XalanDocument* document, XalanNode* contextNode, const string xPath )
  throw( xcept::Exception ){

  XALAN_USING_XALAN(XalanNode);
  XalanNode* theNode;

  XALAN_USING_XALAN(XSLException);

  try{
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport    theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver  thePrefixResolver(document);
    XPathEvaluator               theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theNode = theEvaluator.selectSingleNode( theDOMSupport,
					     contextNode,
					     XalanDOMString( xPath.c_str() ).c_str(),
					     thePrefixResolver );
  }
  catch( xcept::Exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  return theNode;
}

XALAN_USING_XALAN(NodeRefList)

void
emu::farmer::utils::getNodes( NodeRefList& resultNodeList, XalanDocument* document, XalanNode* contextNode, const string xPath )
  throw( xcept::Exception ){

  XALAN_USING_XALAN(XSLException);

  try{
    XALAN_USING_XERCES(XMLPlatformUtils);
    XMLPlatformUtils::Initialize();
    XALAN_USING_XALAN(XPathEvaluator);
    XPathEvaluator::initialize();

    // We'll use these to parse the XML file.
    XALAN_USING_XALAN(XalanSourceTreeDOMSupport);
    XalanSourceTreeDOMSupport    theDOMSupport;
    XALAN_USING_XALAN(XalanDocumentPrefixResolver);
    XalanDocumentPrefixResolver  thePrefixResolver(document);
    XPathEvaluator               theEvaluator;

    XALAN_USING_XALAN(XalanDOMString);
    theEvaluator.selectNodeList( resultNodeList, 
				 theDOMSupport,
				 contextNode,
				 XalanDOMString( xPath.c_str() ).c_str(),
				 thePrefixResolver );
  }
  catch( xcept::Exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': ";
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
  catch( XSLException& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.getMessage();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch( std::exception& e ){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': " << e.what();
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
  catch(...){
    stringstream ss; ss << "XPath selection failed for '"<< xPath << "': Unknown exception.";
    XCEPT_RAISE( xcept::Exception, ss.str() );
  }
}
