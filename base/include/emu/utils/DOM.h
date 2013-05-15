#ifndef __emu_utils_DOM_h__
#define __emu_utils_DOM_h__

//#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMNode.hpp>

#include <xalanc/XalanDOM/XalanNode.hpp>

#include "xcept/Exception.h"


XERCES_CPP_NAMESPACE_USE

namespace emu { namespace utils {


enum Operation_t { replace, add, prepend, append }; ///< To replace old value with new, to add, prepend or append new value to old.

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

    /// Set the value of the node(s) selected by an XPath expression.
    ///
    /// @param XML Original XML string.
    /// @param xPathToNode XPath expression selecting the targeted node(s).
    /// @param value Value to set the node(s) to.
    /// @param operation The operation to be performed on the old value with the new one.
    ///
    /// @return Modified XML string.
    ///
std::string setSelectedNodesValues(const std::string &XML,
				   const std::string &xPathToNode,
				   const std::string &value,
				   Operation_t operation = replace );

    /// Set the value of the node(s) selected by a set of XPath expressions.
    /// The XPath expressions should match disjunct sets of nodes, or else the result will be
    /// hard to predict as they are applied in an alphabetical order.
    ///
    /// @param XML Original XML string.
    /// @param values Container of XPath expression and value pairs.
    /// @param operation The operation to be performed on the old value with the new one.
    ///
    /// @return Modified XML string.
    ///
std::string setSelectedNodesValues(const std::string &XML,
                                   const std::map< std::string, std::string >& values,
				   Operation_t operation = replace );

    /// Get the value of the \e first node selected by an XPath expression.
    /// If \e all selected values need to be returned, use the method \c getSelectedNodesValues.
    ///
    /// @param XML Original XML string.
    /// @param xpath XPath expression selecting the targeted node.
    ///
    /// @return Value of the selected node.
    ///
std::string getSelectedNodeValue(const std::string& XML,
                                 const std::string &xpath);

    /// Get the values of all the nodes selected by a set of XPath expressions.
    ///
    /// @param XML Original XML string.
    /// @param xpath Container of XPath expression and value pairs.
    ///
    /// @return Container of name and value pairs of all selected nodes.
    ///
std::vector< std::pair< std::string, std::string > > getSelectedNodesValues(const std::string& XML,
                                                                            const std::string &xpath);


/**
 *
 */
std::string getSelectedNode(const std::string &XML,
                            const std::string &xpath);

/// Return the value of an attribute node or text content of an element node.
///
/// @param node 
///
/// @return The value.
///
std::string getNodeValue(const DOMNode* const node);

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
