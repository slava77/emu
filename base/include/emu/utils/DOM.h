#ifndef __emu_utils_DOM_h__
#define __emu_utils_DOM_h__

//#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <xalanc/XalanDOM/XalanNode.hpp>

#include "xcept/Exception.h"


XERCES_CPP_NAMESPACE_USE

namespace emu { namespace utils {

/**
 *
 */
std::string serializeDOM(DOMNode* node);

/**
 *
 */
void setNodeValue(DOMNode* node,
                  const std::string& value);

/**
 *
 */
std::string appendToSelectedNode(const std::string &XML,
                                 const std::string &xPathToNode,
                                 const std::string &xmlFragment);

/**
 *
 */
std::string setSelectedNodeValue(const std::string &XML,
                                 const std::string &xPathToNode,
                                 const std::string &value);
/**
 *
 */
std::string setSelectedNodesValues(const std::string &XML,
                                   const std::map< std::string, std::string >& values);

/**
 *
 */
std::string getSelectedNodeValue(const std::string& XML,
                                 const std::string &xpath);

/**
 *
 */
std::vector< std::pair< std::string, std::string > > getSelectedNodesValues(const std::string& XML,
                                                                            const std::string &xpath);


/**
 *
 */
std::string getSelectedNode(const std::string &XML,
                            const std::string &xpath);

XALAN_USING_XALAN(XalanNode)

/**
 *
 */
std::string getNodeValue(const XalanNode* const node);

    /// Removes node(s) selected by an XPath expression.
    ///
    /// @param XML Original XML.
    /// @param xpath XPath expression specifying the node(s) to be removed.
    ///
    /// @return Modified XML.
    ///
std::string removeSelectedNode( const std::string& XML, 
				const std::string& xpath );
}}

#endif
