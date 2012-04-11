// $Id: XMLReadWriter.cc,v 1.2 2012/04/11 21:19:41 khotilov Exp $

#include "emu/db/XMLReadWriter.h"
#include "emu/utils/IO.h"
#include "emu/utils/System.h"
#include "emu/utils/DOM.h"
#include "emu/utils/SimpleTimer.h"

#include "xdata/TableIterator.h"
#include "toolbox/string.h"

#include "xercesc/dom/DOM.hpp"
#include "xercesc/dom/DOMImplementation.hpp"
#include "xercesc/dom/DOMImplementationLS.hpp"
#include "xercesc/dom/DOMWriter.hpp"

#define DBG 0


using namespace emu::utils;

namespace {
#define X(str) xercesc::XMLString::transcode(str)
#define XX(str) std::string(xercesc::XMLString::transcode(str))
}

namespace emu {	namespace db {
	

XMLReadWriter::XMLReadWriter(ConfigHierarchy* hierarchy, const std::string & filename)
: ConfigReadWriter()
, filename_(filename)
, parser_(0)
, document_(0)
, to_serializable_()
, config_id_(9999999)
{
  hierarchy_ = hierarchy;
}


void XMLReadWriter::cleanUpParserAndRaise(const std::string &msg)
throw (emu::exception::ConfigurationException)
{
  if (parser_)
  {
    delete parser_;
    xercesc::XMLPlatformUtils::Terminate();
  }
  XCEPT_RAISE(emu::exception::ConfigurationException, msg);
}


bool XMLReadWriter::read(xdata::UnsignedInteger64 id)
throw (emu::exception::ConfigurationException)
{
  config_id_ = id;

  emu::utils::SimpleTimer timer;

  // prepare empty tables
  datamap_ = hierarchy_->definitionsTree();
  std::vector<std::string> types = hierarchy_->types();

  // Initialize XML4C system
  try
  {
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch (xercesc::XMLException &e)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error during Xerces-c Initialization: " + XX(e.getMessage()));
  }

  // Initialize and configure the parser
  xercesc::XercesDOMParser *parser = new xercesc::XercesDOMParser();
  parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  parser->setCreateEntityReferenceNodes(false);
  //parser->setToCreateXMLDeclTypeNode(true);
  parser->setCreateCommentNodes(false);

  // Parse the XML file, catching any XML exceptions that might propagate out of it.
  try
  {
    parser->parse(filename_.c_str());
  }
  catch (xercesc::XMLException& e)
  {
    cleanUpParserAndRaise("XML Error during parsing: " + XX(e.getMessage()));
  }
  catch (xercesc::DOMException& e)
  {
    cleanUpParserAndRaise("DOM Error during parsing: " + XX(e.getMessage()));
  }
  catch (...)
  {
    cleanUpParserAndRaise("Unknown error during XML parsing");
  }

  // If the parse was successful, output the document data from the DOM tree
  xercesc::DOMNode *doc = parser->getDocument();
  xercesc::DOMElement *head = (xercesc::DOMElement *) doc->getFirstChild();
  // ignore comments
  while (head->getNodeType() == xercesc::DOMNode::COMMENT_NODE)
  {
    head = (xercesc::DOMElement *) head->getNextSibling();
  }
  if (head == NULL)
  {
    cleanUpParserAndRaise("Could not find a top-node in the XML document " + filename_);
  }

  // only the known head node tags are acceptable
  if (!( strcmp(X(head->getTagName()), hierarchy_->typeOfHeadXML().c_str())==0 ||
         strcmp(X(head->getTagName()), hierarchy_->typeOfHead().c_str())==0 ))
  {
    cleanUpParserAndRaise("The name of the head-node " + XX(head->getTagName()) + "in the XML document " +
                          filename_ + " is not in list of known names!");
  }

  if (DBG) std::cout<<"Parsing XML file with head-node name "<<X(head->getTagName())<<std::endl;

  // recursively parse all the nodes starting from the head
  xdata::UnsignedInteger64 parent_id = 0;
  parseNode(head, parent_id, id);

  // Delete the parser itself.  Must be done prior to calling Terminate, below.
  delete parser;

  // And call the termination method
  xercesc::XMLPlatformUtils::Terminate();

  std::cout<<"XML read time: "<<timer.sec()<<" sec"<<std::endl;
  return true;
}


void XMLReadWriter::parseNode(xercesc::DOMNode *node, xdata::UnsignedInteger64 &parent_id, xdata::UnsignedInteger64 &number)
throw (emu::exception::ConfigurationException)
{
  xercesc::DOMElement *element = (xercesc::DOMElement *) node;
  
  std::string type =  X(element->getTagName());
  // provide for the head node having different tag name then head table type
  if (type == hierarchy_->typeOfHeadXML()) type = hierarchy_->typeOfHead();

  if (DBG) std::cout<<" Parsing node "<<type<<std::endl;

  //     parse whatever attributes exist in this node
  //parseNodeAttributes(node, parent_id, number);
  
  // key is column name, value is column type
  typedef std::map<std::string, std::string, xdata::Table::ci_less> DefinitionType;
  DefinitionType &definition = datamap_[type].getTableDefinition();
  //datamap_[type].writeTo(std::cout); std::cout<<std::endl;

  // append new row
  xdata::Table::iterator row = datamap_[type].append();

  // set the common config id
  row->setField(hierarchy_->idFieldNameOfHead(), config_id_);

  // unless it's head node, set parent id and a temporary own ID
  xdata::UnsignedInteger64 own_id = number;
  if (parent_id)
  {
    row->setField(hierarchy_->idFieldNameOfParent(type), parent_id);
    own_id = hierarchy_->id(type, parent_id, number);
    if (DBG) std::cout<<"  node #"<<number.toString()<<"  own id = "<<own_id.toString()<<std::endl;
    row->setField(hierarchy_->idFieldName(type), own_id);
  }

  // fill data columns for the present attributes
  xercesc::DOMNamedNodeMap * attributes = element->getAttributes();
  if (attributes) for (unsigned int i = 0; i < attributes->getLength(); i++)
  {
    xercesc::DOMAttr *attr = (xercesc::DOMAttr *) attributes->item(i);
    std::string attr_name = X(attr->getName());
    std::string value = X(attr->getNodeValue());
    if (DBG) std::cout<<"    got attribute "<<attr_name <<" = "<<value;
    translatorKludge_.XML2DB(attr_name, value);
    //std::cout<<"          kludged "<<attr_name <<" = "<<value<<std::endl;

    if (definition.find(attr_name) == definition.end())
    {
      //std::ostringstream error;
      std::cout << "Attribute name " << attr_name <<"="<<value << " is not in definition:"<<std::endl;
      datamap_[type].writeTo(std::cout);
      std::cout<<std::endl<<"... skipping the attribute ..."<<std::endl;
      continue;
      //XCEPT_RAISE(emu::exception::ConfigurationException, error.str());
    }
    std::string column_type = definition[attr_name];
    if (DBG) std::cout<<" ("<<column_type<<")"<<std::endl;

    xdata::Serializable * serializable = to_serializable_(column_type, value);
    row->setField(attr_name, *serializable);
    delete serializable;
  }
  //datamap_[type].writeTo(std::cout); std::cout<<std::endl;


  //   recursively deal with children nodes in a depth-first manner
  std::vector< std::string > kid_types = hierarchy_->typesOfChildern(type);
  for (std::vector< std::string >::iterator t = kid_types.begin(); t != kid_types.end(); t++)
  {
    xercesc::DOMNodeList *kid_type_nodes = element->getElementsByTagName( X( (*t).c_str() ) );
    xdata::UnsignedInteger64 kid_type_counter = 0;
    if (kid_type_nodes) for (unsigned int i = 0; i < kid_type_nodes->getLength(); i++)
    {
      xercesc::DOMElement *kid = (xercesc::DOMElement *) kid_type_nodes->item(i);
      parseNode(kid, own_id, kid_type_counter);
      kid_type_counter++;
    }
  }
}


bool XMLReadWriter::write(std::map<std::string, ConfigTable> *data)
throw (emu::exception::ConfigurationException)
{
  ConfigTree *tree = 0;
  // create ConfigTree either from the datamap_ (if zero pointer) or from data
  if (data == 0) tree = new ConfigTree(datamap_);
  else           tree = new ConfigTree(*data);
  bool result = write(tree);
  delete tree;
  return result;
}


bool XMLReadWriter::write(ConfigTree *tree)
throw (emu::exception::ConfigurationException)
{
  std::string xml = makeXML(tree);
  try
  {
    emu::utils::writeFile(filename_, xml);
  }
  catch (xcept::Exception &e)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Cannot write xml to the file " + std::string(e.what()) );
  }
  return true;
}


std::string XMLReadWriter::makeXML(ConfigTree *tree)
throw (emu::exception::ConfigurationException)
{
  try // Initialize XML4C system
  {
    xercesc::XMLPlatformUtils::Initialize();
  }
  catch (xercesc::XMLException &e)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error during Xerces-c Initialization: " + XX(e.getMessage()) );
  }

  // An implementation to build a DOMWriter.  The implementation with LS feature (LoadSave) is good enough.
  xercesc::DOMImplementation *implementation = xercesc::DOMImplementationRegistry::getDOMImplementation(X("LS"));
  if (implementation == NULL)
  {
    xercesc::XMLPlatformUtils::Terminate();
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error getting DOM implementation: feature 'Core' unsupported");
  }

  // Create the document
  try
  {
    // arguments: root element namespace URI, root element name, document type object (DTD).
    document_ = implementation->createDocument(NULL, X(hierarchy_->typeOfHeadXML().c_str()), NULL);
  }
  catch (xercesc::DOMException& e)
  {
    xercesc::XMLPlatformUtils::Terminate();
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error creating DOMDocument: " + XX(e.getMessage()));
  }

  // Get the newly created root
  xercesc::DOMElement *root;
  try
  {
    root = (xercesc::DOMElement *) document_->getFirstChild();
  }
  catch (xercesc::DOMException& e)
  {
    xercesc::XMLPlatformUtils::Terminate();
    XCEPT_RAISE(emu::exception::ConfigurationException, "Error getting root element: " + XX(e.getMessage()));
  }

  // find the head configuration row:
  ConfigRow * head = tree->head();
  if (head == 0)
  {
    XCEPT_RAISE(emu::exception::ConfigurationException, "XML write: no head row found in ConfigTree");
  }

  // recursively create DOM nodes
  fillNode(root, head);

  // Create a writer
  xercesc::DOMWriter *writer = ((xercesc::DOMImplementationLS *) implementation)->createDOMWriter();

  // Make things pretty if we can
  if (writer->canSetFeature(X("format-pretty-print"), true)) writer->setFeature(X("format-pretty-print"), true);

  // Write
  std::string return_xml = X(writer->writeToString(*document_));

  delete writer;

  // Release all memory used by the document
  document_->release();

  // Terminate XML services
  xercesc::XMLPlatformUtils::Terminate();

  // a hack to have encoding="UTF-8" instead of UTF-16, as for some reason Xercesc's setEncoding methods can't really do that
  std::string to_replace = "encoding=\"UTF-16\"";
  size_t found = return_xml.find(to_replace);
  if (found != std::string::npos) return_xml.replace(found, to_replace.length(), "encoding=\"UTF-8\"");

  return return_xml;
}


void XMLReadWriter::fillNode(xercesc::DOMNode *node, ConfigRow *row)
throw (emu::exception::ConfigurationException)
{
  xercesc::DOMElement *element = (xercesc::DOMElement *) node;

  std::string tag =  row->type();
  // provide for the head node having different tag name then head table type
  if (tag == hierarchy_->typeOfHead()) tag = hierarchy_->typeOfHeadXML();

  if (DBG) std::cout<<" Filling node "<<tag<<std::endl;

  // set node attributes
  xdata::Table data = row->row();
  std::vector<std::string> fields = data.getColumns();
  for (size_t i = 0; i < fields.size(); i++ )
  {
    // no attributed are created from DB only fields
    if(hierarchy_->isFieldDBOnly(row->type(), fields[i])) continue;

    std::string field_name = fields[i];
    std::string field_value;
    try
    {
      field_value = data.getValueAt(0, fields[i])->toString();
    }
    catch (xdata::exception::Exception &e)
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unable to get value of " + row->type() + ":" + field_name + ": " + std::string(e.what()) );
    }
    if (DBG) std::cout<<"    got field "<<field_name <<" = "<<field_value;
    translatorKludge_.DB2XML(field_name, field_value);
    if (DBG) std::cout<<"          kludged "<<field_name <<" = "<<field_value<<std::endl;
    try
    {
      element->setAttribute(X(field_name.c_str()), X(field_value.c_str()));
    }
    catch (xercesc::DOMException &e)
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unable to set attributes for " + tag + ": " + XX(e.getMessage()) );
    }
  }

  //   recursively deal with children rows in a depth-first manner
  std::vector<ConfigRow* > kids = row->children();
  for (std::vector<ConfigRow* >::iterator kid = kids.begin(); kid != kids.end(); kid++)
  {
    xercesc::DOMElement *kid_element = 0;

    try // crate a child element
    {
      kid_element = document_->createElement(X((*kid)->type().c_str()));
    }
    catch (xercesc::DOMException &e)
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unable to create " + (*kid)->type() + " element: " + XX(e.getMessage()) );
    }

    // Append to the node
    xercesc::DOMElement *kid_node = 0;
    try
    {
      kid_node = (xercesc::DOMElement *) node->appendChild(kid_element);
    }
    catch (xercesc::DOMException &e)
    {
      XCEPT_RAISE(emu::exception::ConfigurationException, "Unable add child " + (*kid)->type() + " to the node " + tag + ": " + XX(e.getMessage()) );
    }

    fillNode(kid_node, *kid);
  }
}

}}
