#!/usr/bin/env python


try:
    import xml.etree.ElementTree as ET
except ImportError:
    import elementtree.ElementTree as ET

import os, sys, optparse, time


def xml_compare(e1, e2, reporter=None, children_order=True, recursive=True):
    '''
Compares xml elements e1 and e2.
Optional parameters: 
  reporter         a functor to store detailed comparison results
  children_order   whether to take the order of element's children into account when comparing
  recursive        recursively compare all the descendants 
    ''' 
    ### the same tag
    if e1.tag != e2.tag:
        if reporter:
            reporter('Tags do not match: %s and %s' % (e1.tag, e2.tag))
        return False
    
    ### compare elemant's text contents
    for name, value in e1.attrib.items():
        if e2.attrib.get(name) != value:
            if reporter:
                reporter('Attributes do not match: %s=%r, %s=%r'
                         % (name, value, name, e2.attrib.get(name)))
            return False
    for name in e2.attrib.keys():
        if name not in e1.attrib:
            if reporter:
                reporter('e2 has an attribute e1 is missing: %s' % name)
            return False

    ### compare elemant's text contents
    if not text_compare(e1.text, e2.text):
        if reporter:
            reporter('text: %r != %r' % (e1.text, e2.text))
        return False
    
    ### compare contents between this element and the next sibling
    if not text_compare(e1.tail, e2.tail):
        if reporter:
            reporter('tail: %r != %r' % (e1.tail, e2.tail))
        return False
    
    ### move onto children
    cl1 = e1.getchildren()
    cl2 = e2.getchildren()

    ### the same number of children check
    if len(cl1) != len(cl2):
        if reporter:
            reporter('children length differs, %i != %i' % (len(cl1), len(cl2)))
        return False
    
    ### recursive children comparison
    i = 0
    if children_order:
        for c1, c2 in zip(cl1, cl2):
            i += 1
            if not xml_compare(c1, c2, reporter=reporter):
                if reporter:
                    reporter('child %i does not match: %s' % (i, c1.tag))
                return False
    else: # TODO improve the usefulness of the reporter for this case 
        for c1 in cl1:
            i += 1
            found = False
            for c2 in cl2:
                if xml_compare(c1, c2, reporter=None, children_order=children_order, recursive=False):
                    found = True
                    xml_compare(c1, c2, reporter=reporter, children_order=children_order)
                    break
            if not found:
                if reporter:
                    reporter('child c1# %i has no match: %s' % (i, c1.tag))
                return False
    return True


def text_compare(t1, t2):
    if not t1 and not t2:
        return True
    if t1 == '*' or t2 == '*':
        return True
    return (t1 or '').strip() == (t2 or '').strip()



usage = '''
  diff2XMLs.py [options] xml1.py xml2.py
Find whether two xmls differ in contents.
'''

optparser = optparse.OptionParser(usage)

optparser.add_option("-c","--noChildrenOrder",
  help="if present, specifies that the order of element's children is not important",
  action="store_true",
  default=False,
  dest="noChildrenOrder")

optparser.add_option("-t","--timing",
  help="print execution timing",
  action="store_true",
  default=False,
  dest="timing")

options,args = optparser.parse_args()

CHILDREN_ORDER_MATTERS = True
if options.noChildrenOrder:
    CHILDREN_ORDER_MATTERS = False

if len(args) != 2:
    optparser.print_help()

for f in args:
  if not os.access(f, os.F_OK):
    print f, "  does not exist! Exiting..."
    sys.exit()

t0 = time.time()
xml1 = ET.XML(open(args[0],'r').read())
xml2 = ET.XML(open(args[1],'r').read())

t1 = time.time()
if options.timing:
    print "files read in   %.4f sec" % (t1-t0)

result = []
cm = xml_compare(xml1, xml2, result.append, children_order=CHILDREN_ORDER_MATTERS)
t2 = time.time()
if options.timing: 
    print "comparison took %.4f sec" % (t2-t1)

if len(result): print result
