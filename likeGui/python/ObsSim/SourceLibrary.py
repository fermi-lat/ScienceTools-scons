import os
from xml.dom import minidom

def readXml(file):
    infile = open(file, 'r')
    lines = infile.readlines()
    lines.insert(0, '<bibliotheque>\n')
    lines.append('</bibliotheque>\n')
    return ''.join(lines)

class SourceLibrary(dict):
    def __init__(self, file):
        self.file = os.path.abspath(file)
        basename = os.path.basename(file).split('.xml')[0]
        doc = minidom.parseString(readXml(file))
        srcLibs = doc.getElementsByTagName('source_library')
        for lib in srcLibs:
            title = lib.getAttribute('title').encode()
            if title:
                name = ": ".join((basename, title))
            else:
                name = basename
            self[name] = lib
    def srcList(self, lib):
        names = []
        srcs = self[lib].getElementsByTagName('source')
        for src in srcs:
            names.append(src.getAttribute('name').encode())
        return names

if __name__ == '__main__':
    library = SourceLibrary('source_library.xml')
    for src in library.srcList('background'):
        print src
        
