#!/usr/bin/env python

import sys
from xml.dom import minidom

def cleanXml(document):
    try:
        doc = minidom.parse(document)
    except AttributeError:
        doc = document
    lines = doc.toxml().split('>')
    clean_lines = []
    for line in lines:
        if line.find('<') != -1:
            line = line + '>'
            clean_lines.append(line.strip())
    xmlString = '\n'.join(clean_lines)
    new_doc = minidom.parseString(xmlString)
    return new_doc

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "usage: cleanXml <xml file name>"
        sys.exit(0)
    else:
        filename = sys.argv[1]
        doc = cleanXml(filename)
        file = open(filename, 'w')
        file.write(doc.toxml() + '\n')
        file.close()

