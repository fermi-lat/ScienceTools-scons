#!/usr/bin/env python
"""
Prototype GUI for obsSim

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#

import os
import sys
import copy
import string
import Tkinter as Tk
from FileDialog import LoadFileDialog
from tkMessageBox import showwarning

sys.path.insert(0, os.path.join(os.environ['LIKEGUIROOT'], 'python'))

from SourceLibrary import SourceLibrary
from ParamDialog import ParamDialog
from EditFileDialog import *
from ThreadedClient import runInThread
from pil import Pil

# @todo replace these with GtApp objects
obsSim = os.path.join(os.environ["OBSERVATIONSIMROOT"],
                      os.environ["BINDIR"], 'obsSim.exe')
orbSim = os.path.join(os.environ["OBSERVATIONSIMROOT"],
                      os.environ["BINDIR"], 'orbSim.exe')

class RootWindow(Tk.Tk):
    def __init__(self, file=None):
        Tk.Tk.__init__(self)
        self.title("obsSim GUI")
        menuBar = MenuBar(self)

        inputFrames = Tk.Frame(self)
        inputFrames.pack(side=Tk.LEFT, fill=Tk.BOTH)
        self.srcLibs = SourceLibraries(self, inputFrames)
        self.candidates = CandidateSources(self, inputFrames)

        outputFrame = Tk.Frame(self)
        outputFrame.pack(side=Tk.RIGHT, fill=Tk.BOTH)
        self.sourceList = SourceList(self, outputFrame)

        if file is not None:
            self.srcLibs.addLibs(file)
        else:
            xmlFiles = os.path.join(os.environ['OBSERVATIONSIMROOT'], 'xml',
                                    'xmlFiles.dat')
            defaultFiles = open(xmlFiles, 'r')
            for file in defaultFiles:
                if file.strip() != '':
                    self.srcLibs.addLibs(expandEnvVar(file).strip())
    def open(self, xmlFile=None):
        if xmlFile is None:
            xmlFile = LoadFileDialog(self).go(pattern='*.xml')
        if xmlFile is not None:
            self.srcLibs.addLibs(xmlFile)
    def obsSim(self, xmlList='xmlFiles.dat', sourceNames='source_names.dat'):
        self.srcLibs.writeXmlFileList(xmlList)
        self.sourceList.writeSourceNames(sourceNames)
        pars = Pil('obsSim.par')
        pars['xml_source_file'] = xmlList
        pars['source_list'] = sourceNames
        pfile = 'obsSim.par'
        pars.write(pfile)   # create a local copy
        dialog = ParamDialog(self, pfile)
        if dialog.paramString:
            command = " ".join((obsSim, dialog.paramString))
            runInThread(commandApp(command))
    def orbSim(self):
        pars = Pil('orbSim.par')
        pfile = 'orbSim.par'
        pars.write(pfile)   # create a local copy
        dialog = ParamDialog(self, pfile)
        if dialog.paramString:
            command = " ".join((orbSim, dialog.paramString))
            runInThread(commandApp(command))
        
class MenuBar(Tk.Menu):
    def __init__(self, parent):
        Tk.Menu.__init__(self)
        self.add_cascade(label="File", menu=FileMenu(parent), underline=0)
        self.add_cascade(label="ds9", menu=Ds9Menu(parent), underline=0)
        parent.config(menu=self)

class FileMenu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0)
        self.add_command(label="Open...", underline=0, command=root.open)
        self.add_cascade(label="Run", underline=0, menu=SimMenu(root))
        self.add_command(label="Quit", underline=0, command=root.quit)
        
class SimMenu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0)
        self.add_command(label="obsSim...", command=root.obsSim, underline=1)
        self.add_command(label="orbSim...", command=root.orbSim, underline=1)

class Ds9Menu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0)
        self.add_command(label="Events...", underline=0, command=Ds9(root, 1))
        self.add_command(label="Image...", underline=0, command=Ds9(root))

class Ds9(object):
    def __init__(self, root, events=0):
        self.root = root
        self.events = events
    def __call__(self):
        file = EditFileDialog(self.root).go(pattern='*.fits')
        if file:
            if self.events:
                command = 'ds9 "' + file + '[1][bin=ra,dec]"'
            else:
                command = 'ds9 ' + file
            runInThread(commandApp(command))

class SourceLibraries(Tk.Frame):
    def __init__(self, root, parentFrame):
        self.root = root
        self.parentFrame = parentFrame
        Tk.Frame.__init__(self)
        libFrame = Tk.Frame(self.parentFrame)
        libFrame.pack(side=Tk.TOP, fill=Tk.BOTH)

        self.menuButton = Tk.Menubutton(libFrame, text='Source Libraries',
                                        relief=Tk.RAISED, bd=2, underline=0)
        self.menuButton.pack(side=Tk.TOP, fill=Tk.X)
        self.menuButton['menu'] = SourceLibMenu(self, self.menuButton)

        self.xScroll = Tk.Scrollbar(libFrame, orient=Tk.HORIZONTAL)
        self.xScroll.pack(side=Tk.BOTTOM, fill=Tk.X)
        self.yScroll = Tk.Scrollbar(libFrame, orient=Tk.VERTICAL)
        self.yScroll.pack(side=Tk.RIGHT, fill=Tk.Y)
        self.listBox = Tk.Listbox(libFrame, selectmode=Tk.BROWSE,
                                  xscrollcommand=self.xScroll.set,
                                  yscrollcommand=self.yScroll.set,
                                  width=30)
        self.listBox.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)
        btags = self.listBox.bindtags()
        self.listBox.bindtags(btags[1:] + btags[:1])
        self.listBox.bind('<ButtonRelease-1>', self.selectLibrary)
        self.xScroll["command"] = self.listBox.xview
        self.yScroll["command"] = self.listBox.yview
        self.libs = {}
        self.files = []
    def fill(self):
        self.listBox.delete(0, Tk.END)
        for lib in self.libs:
            self.listBox.insert(Tk.END, lib)
    def addLibs(self, file):
        srcLibs = SourceLibrary(file)
        file = os.path.abspath(file)
        for lib in srcLibs:
            if lib not in self.libs:
                self.libs[lib] = (srcLibs[lib], file)
        self.fill()
        if file not in self.files:
            self.files.append(file)
    def selectLibrary(self, event=None):
        libname = self.listBox.get('active')
        self.root.candidates.setSrcLib(self.libs[libname][0], libname)
    def deleteSelected(self):
        name = self.listBox.get('active')
        targetfile = self.libs[name][1]
        self.files.remove(targetfile)
        targets = self.libs.keys()
        for lib in targets:
            if self.libs[lib][1] == targetfile:
                del self.libs[lib]
        self.fill()
        self.root.candidates.clearAll()
        self.root.candidates.menuButton.config(text='Candidate Sources')
    def writeXmlFileList(self, file='xmlFiles.dat'):
        outfile = open(file, 'w')
        for item in self.files:
            outfile.write(item + "\n")
        outfile.close()

class SourceLibMenu(Tk.Menu):
    def __init__(self, root, parentFrame):
        Tk.Menu.__init__(self, parentFrame, tearoff=0)
        self.add_command(label='Open...', underline=0,
                         command=root.root.open)
        self.add_command(label='Delete selected', underline=0,
                         command=root.deleteSelected)

class CandidateSources(Tk.Frame):
    def __init__(self, root, parentFrame):
        self.root = root
        self.parentFrame = parentFrame
        Tk.Frame.__init__(self)
        srcFrame = Tk.Frame(self.parentFrame)
        srcFrame.pack(side=Tk.TOP, fill=Tk.BOTH)

        self.menuButton = Tk.Menubutton(srcFrame, text='Candidate Sources',
                                        relief=Tk.RAISED, bd=2, underline=0)
        self.menuButton.pack(side=Tk.TOP, fill=Tk.X)
        self.menuButton['menu'] = CandidateMenu(self, self.menuButton)

        self.xScroll = Tk.Scrollbar(srcFrame, orient=Tk.HORIZONTAL)
        self.xScroll.pack(side=Tk.BOTTOM, fill=Tk.X)
        self.yScroll = Tk.Scrollbar(srcFrame, orient=Tk.VERTICAL)
        self.yScroll.pack(side=Tk.RIGHT, fill=Tk.Y)
        self.listBox = Tk.Listbox(srcFrame, selectmode=Tk.MULTIPLE,
                                  xscrollcommand=self.xScroll.set,
                                  yscrollcommand=self.yScroll.set,
                                  width=30)
        self.listBox.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)
        btags = self.listBox.bindtags()
        self.listBox.bindtags(btags[1:] + btags[:1])
        self.listBox.bind('<Button-3>', self.printXml)
        self.listBox.bind('<Double-ButtonRelease-1>', self.addSource)
        self.xScroll["command"] = self.listBox.xview
        self.yScroll["command"] = self.listBox.yview
    def setSrcLib(self, srcLib, libName):
        if len(libName) > 30:
            libName = libName[:30]
        self.menuButton.config(text=libName)
        srcElts = srcLib.getElementsByTagName('source')
        self.sources = map()
        for src in srcElts:
            name = src.getAttribute('name').encode()
            self.sources[name] = src
        self.listBox.delete(0, Tk.END)
        for src in self.sources.ordered_keys:
            self.listBox.insert(Tk.END, src)
    def addSource(self, event):
        src = self.listBox.get('active')
        self.root.sourceList.addSource((src, self.sources[src]))
    def addSelected(self):
        selected = self.selectedSources()
        for srcName in selected:
            srcName = self.sources.ordered_keys[int(indx)]
            self.root.sourceList.addSource((srcName, self.sources[srcName]))
    def selectedSources(self):
        indices = self.listBox.curselection()
        selected = []
        for indx in indices:
            selected.append(self.sources.ordered_keys[int(indx)])
        return selected        
    def addAll(self):
        self.selectAll()
        self.addSelected()
    def selectAll(self):
        self.listBox.selection_set(0, self.listBox.size())
    def unSelectAll(self):
        self.listBox.selection_clear(0, self.listBox.size())
    def clearAll(self):
        self.listBox.delete(0, Tk.END)
        self.root.sourceList.deleteAll()
    def printXml(self):
        selected = self.selectedSources()
        for name in selected:
            print self.sources[name].toxml() + '\n'

class CandidateMenu(Tk.Menu):
    def __init__(self, root, parentFrame):
        Tk.Menu.__init__(self, parentFrame, tearoff=0)
        self.add_command(label='Add selected', underline=0,
                         command=root.addSelected)
        self.add_command(label='Add all', command=root.addAll)
        self.add_command(label='Select all', underline=0,
                         command=root.selectAll)
        self.add_command(label='Un-select all', underline=0,
                         command=root.unSelectAll)
        self.add_command(label='Print xml', underline=0,
                         command=root.printXml)

class SourceList(Tk.Frame):
    def __init__(self, root, parentFrame):
        Tk.Frame.__init__(self)
        self.root = root
        srcFrame = Tk.Frame(parentFrame)
        srcFrame.pack(side=Tk.RIGHT, fill=Tk.BOTH)

        self.menuButton = Tk.Menubutton(srcFrame, text='Model Sources',
                                        relief=Tk.RAISED, bd=2, underline=0)
        self.menuButton.pack(side=Tk.TOP, fill=Tk.X)
        self.menuButton['menu'] = ModelMenu(self, self.menuButton)

        self.xScroll = Tk.Scrollbar(srcFrame, orient=Tk.HORIZONTAL)
        self.xScroll.pack(side=Tk.BOTTOM, fill=Tk.X)
        self.yScroll = Tk.Scrollbar(srcFrame, orient=Tk.VERTICAL)
        self.yScroll.pack(side=Tk.RIGHT, fill=Tk.Y)
        self.listBox = Tk.Listbox(srcFrame, selectmode=Tk.MULTIPLE,
                                  xscrollcommand=self.xScroll.set,
                                  yscrollcommand=self.yScroll.set,
                                  width=30)
        self.listBox.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)
        btags = self.listBox.bindtags()
        self.listBox.bindtags(btags[1:] + btags[:1])
        self.listBox.bind('<Double-ButtonRelease-1>', self.printXml)
        self.xScroll["command"] = self.listBox.xview
        self.yScroll["command"] = self.listBox.yview
        self.srcs = map()
    def addSource(self, src):
        if src[0] not in self.srcs:
            self.srcs[src[0]] = src[1]
        self.fill()
    def fill(self):
        self.listBox.delete(0, Tk.END)
        for src in self.srcs.ordered_keys:
            self.listBox.insert(Tk.END, src)
    def deleteSelected(self):
        indices = self.listBox.curselection()
        targets = []
        for indx in indices:
            targets.append(self.srcs.ordered_keys[int(indx)])
        for target in targets:
            self.srcs.delete(target)
        self.fill()
    def deleteAll(self):
        self.selectAll()
        self.deleteSelected()
    def selectAll(self):
        self.listBox.selection_set(0, self.listBox.size())
    def unSelectAll(self):
        self.listBox.selection_clear(0, self.listBox.size())
    def writeSourceNames(self, file='source_names.dat'):
        outfile = open(file, 'w')
        for src in self.srcs.ordered_keys:
            outfile.write(src + "\n")
        outfile.close()
    def printXml(self, event):
        name = self.listBox.get('active')
        print self.srcs[name].toxml() + '\n'

class ModelMenu(Tk.Menu):
    def __init__(self, root, parentFrame):
        Tk.Menu.__init__(self, parentFrame, tearoff=0)
        self.add_command(label='Delete selected', underline=0,
                         command=root.deleteSelected)
        self.add_command(label='Delete all', underline=0,
                         command=root.deleteAll)
        self.add_command(label='Select all', underline=0,
                         command=root.selectAll)
        self.add_command(label='Un-select all', underline=0,
                         command=root.unSelectAll)
        
class commandApp:
    def __init__(self, command):
        self.command = command
    def __call__(self):
        output = os.system(self.command)

class map(dict):
    def __init__(self):
        dict.__init__(self)
        self.ordered_keys = []
    def __setitem__(self, key, value):
        dict.__setitem__(self, key, value)
        if key not in self.ordered_keys:
            self.ordered_keys.append(key)
    def delete(self, key):
        self.ordered_keys.remove(key)        
        del self[key]

def expandEnvVar(filename):
    if filename.find('$(') != -1:
        envVar = filename.split('$(')[1].split(')')[0]
        remainder = filename.split('$(')[1].split(')')[1]
        return os.environ[envVar] + remainder
    else:
        return filename

if __name__ == "__main__":
    if sys.argv[1:]:
        root = RootWindow(sys.argv[1])
    else:
        root = RootWindow()
    root.mainloop()
