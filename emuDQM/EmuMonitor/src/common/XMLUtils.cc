#include "XMLUtils.h"

using namespace XERCES_CPP_NAMESPACE;

DOMElement* makeElement(DOMDocument*& doc, string tag, string val)
{
  DOMElement* elem = doc->createElement(X(tag));
  DOMText*    value = doc->createTextNode(X(val));
  elem->appendChild(value);
  return elem; 
}

