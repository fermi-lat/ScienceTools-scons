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

sys.path.insert(0, os.path.join(os.environ['LIKEGUIROOT'], 'python'))

from FileDialog import LoadFileDialog
from tkMessageBox import showwarning

from SourceLibrary import SourceLibrary
from ParamDialog import ParamDialog
from pil import Pil

ds9 = None

# @todo replace this with a GtApp object
obsSim = os.path.join(os.environ["OBSERVATIONSIMROOT"],
                      os.environ["BINDIR"], 'obsSim.exe')

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
    def open(self, xmlFile=None):
        if xmlFile is None:
            xmlFile = LoadFileDialog(self).go(pattern='*.xml')
        self.srcLibs.addLibs(xmlFile)
    def run(self, xmlList='xmlFiles.dat', sourceNames='source_names.dat'):
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
            os.system(command)
        
class MenuBar(Tk.Menu):
    def __init__(self, parent):
        Tk.Menu.__init__(self)
        self.add_cascade(label="File", menu=FileMenu(parent), underline=0)
        if ds9 is not None:
            self.add_cascade(label="ds9", menu=ds9Menu(parent), underline=0)
        parent.config(menu=self)

class FileMenu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0)
        self.root = root
        self.add_command(label="Open...", underline=0, command=root.open)
        self.add_command(label="Run obsSim...", command=root.run)
        self.add_command(label="Quit", underline=0, command=root.quit)

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
        self.root.candidates.setSrcLib(self.libs[libname][0])
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
        self.listBox.bind('<Double-ButtonRelease-1>', self.addSource)
        self.xScroll["command"] = self.listBox.xview
        self.yScroll["command"] = self.listBox.yview
    def setSrcLib(self, srcLib):
        srcElts = srcLib.getElementsByTagName('source')
        self.sources = []
        for src in srcElts:
            self.sources.append(src.getAttribute('name').encode())
        self.listBox.delete(0, Tk.END)
        for src in self.sources:
            self.listBox.insert(Tk.END, src)
    def addSource(self, event):
        src = self.listBox.get('active')
        self.root.sourceList.addSource(src)
    def addSelected(self):
        indices = self.listBox.curselection()
        for indx in indices:
            self.root.sourceList.addSource(self.sources[int(indx)])
    def selectAll(self):
        self.listBox.selection_set(0, self.listBox.size())
    def unSelectAll(self):
        self.listBox.selection_clear(0, self.listBox.size())
    def clearAll(self):
        self.listBox.delete(0, Tk.END)

class CandidateMenu(Tk.Menu):
    def __init__(self, root, parentFrame):
        Tk.Menu.__init__(self, parentFrame, tearoff=0)
        self.add_command(label='Add selected', underline=0,
                         command=root.addSelected)
        self.add_command(label='Select all', underline=0,
                         command=root.selectAll)
        self.add_command(label='Un-select all', underline=0,
                         command=root.unSelectAll)

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
        self.xScroll["command"] = self.listBox.xview
        self.yScroll["command"] = self.listBox.yview
        self.srcs = map()
    def addSource(self, src):
        if src not in self.srcs:
            self.srcs[src] = src
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
            self.srcs.delete(self.srcs[target])
        self.fill()
    def selectAll(self):
        self.listBox.selection_set(0, self.listBox.size())
    def unSelectAll(self):
        self.listBox.selection_clear(0, self.listBox.size())
    def writeSourceNames(self, file='source_names.dat'):
        outfile = open(file, 'w')
        for src in self.srcs.ordered_keys:
            outfile.write(src + "\n")
        outfile.close()

class ModelMenu(Tk.Menu):
    def __init__(self, root, parentFrame):
        Tk.Menu.__init__(self, parentFrame, tearoff=0)
        self.add_command(label='Delete selected', underline=0,
                         command=root.deleteSelected)
        self.add_command(label='Select all', underline=0,
                         command=root.selectAll)
        self.add_command(label='Un-select all', underline=0,
                         command=root.unSelectAll)

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

if __name__ == "__main__":
    if sys.argv[1:]:
        root = RootWindow(sys.argv[1])
    else:
        root = RootWindow()
    root.mainloop()
