#ifndef __emu_utils_Xalan_h__
#define __emu_utils_Xalan_h__

#include <xalanc/XalanDOM/XalanDocument.hpp>
#include <xalanc/XalanDOM/XalanNode.hpp>
#include <xalanc/XPath/NodeRefList.hpp>

#include <iostream>
#include <map>

namespace emu { namespace utils {

/**
 * transform source using stylesheet and output the result into target
 */
int transformStreams( std::istream& source,
                      std::istream& stylesheet,
                      std::ostream& target );

/**
 *
 */
int transformWithParams( std::istream& source,
                         std::istream& stylesheet,
                         std::ostream& target,
                         const std::map< std::string, std::string > &params );

XALAN_USING_XALAN(XalanDocument)
XALAN_USING_XALAN(XalanNode)

/**
 * node to string of formatted XML
 */
std::string serialize( const XalanNode* node );

/**
 * retrieve single node matching xpath
 */
XalanNode* getSingleNode( XalanDocument* doc,
                          XalanNode* contextNode,
                          const std::string &xPath );

XALAN_USING_XALAN(NodeRefList)

/**
 * retrieve multiple nodes matching xpath
 */
void getNodes( NodeRefList& resultNodeList,
               XalanDocument* document,
               XalanNode* contextNode,
               const std::string &xPath );

/**
 *
 */
std::string serializeSelectedNode( const std::string& XML,
                                   const std::string& xPath );

}}

#endif
