#!/usr/bin/env python
"""
Prototype source model editor.

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
import mySimpleDialog
from FileDialog import LoadFileDialog, SaveFileDialog
import readXml
import FuncFactory as funcFactory

class RootWindow(Tk.Tk):
    def __init__(self, spectralFuncs, spatialFuncs):
        Tk.Tk.__init__(self)
        self.srcModel = readXml.SourceModel()
        self.title("Source Model Editor")
        self.ptsrcs = 0
        self.difsrcs = 0

        menuBar = MenuBar(self)
        
        self.modelEditor = ModelEditor(self)
        self.modelEditor.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)

        componentFrames = Tk.Frame(self)
        componentFrames.pack()

        sourceFrame = Tk.Frame(componentFrames)
        sourceFrame.pack()
        self.currentSource = Tk.StringVar()
        currentSrcEntry = Tk.Entry(sourceFrame,
                                   textvariable=self.currentSource)
        currentSrcEntry.bind('<Return>', self.setSourceName)
        currentSrcEntry.pack(side=Tk.LEFT)

        setSrcButton = Tk.Button(sourceFrame, text='Set components', bd=1,
                                 command=self.exportSrcComponents)
        setSrcButton.pack(side=Tk.RIGHT)
        
        Tk.Label(componentFrames, text="").pack()
        spectrumFrame = Tk.Frame(componentFrames)
        spectrumFrame.pack()
        self.spectrum = ComponentEditor(spectrumFrame, spectralFuncs,
                                        "Spectrum")
        Tk.Label(componentFrames, text="").pack()
        spatialFrame = Tk.Frame(componentFrames)
        spatialFrame.pack()
        self.spatialModel = ComponentEditor(spatialFrame, spatialFuncs,
                                            "Spatial Model")
    def setSourceName(self, event):
        try:
            srcName = self.modelEditor.currentSrcName
            newName = self.currentSource.get()
            if srcName != newName:
                srcList = list(self.modelEditor.listBox.get(0, Tk.END))
                indx = srcList.index(srcName)
                self.modelEditor.listBox.selection_set(indx)
                self.srcModel[newName] = copy.deepcopy(self.srcModel[srcName])
                self.srcModel[newName].node.setAttribute('name', newName)
                self.srcModel[newName].name = newName
                del self.srcModel[srcName]
                self.modelEditor.fill()
                self.modelEditor.selectSource()
                srcList = list(self.modelEditor.listBox.get(0, Tk.END))
                indx = srcList.index(srcName)
                self.modelEditor.listBox.selection_set(indx)
        except (ValueError, AttributeError):
            pass
    def importSrcComponents(self, srcName):
        src = self.srcModel[srcName]
        for name in self.spectrum.functionDict.keys():
            func = self.spectrum.functionDict[name]
            if src.spectrum.type == func.type:
                self.spectrum.functionDict[name] = src.spectrum
        for name in self.spatialModel.functionDict.keys():
            func = self.spatialModel.functionDict[name]
            if src.spatialModel.type == func.type:
                self.spatialModel.functionDict[name] = src.spatialModel
    def exportSrcComponents(self):
        try:
            src = self.srcModel[self.currentSource.get()]
            cp = copy.deepcopy
            specFunc = self.spectrum.currentFunc.get()
            src.spectrum = cp(self.spectrum.functionDict[specFunc])
            src.deleteChildElements('spectrum')
            src.node.appendChild(src.spectrum.node)
            spatialFunc = self.spatialModel.currentFunc.get()
            src.spatialModel = cp(self.spatialModel.functionDict[spatialFunc])
            src.deleteChildElements('spatialModel')
            src.node.appendChild(src.spatialModel.node)
            self.syncSrcType(src)
        except:
            pass
    def syncSrcType(self, src):
        if src.spatialModel.type == "SkyDirFunction":
            src.type = "PointSource"
        else:
            src.type = "DiffuseSource"
        src.setAttributes()
    def open(self):
        xmlFile = LoadFileDialog(self).go(pattern='*.xml')
        self.srcModel = readXml.SourceModel(xmlFile)
        self.modelEditor.fill()
        self.title('Source Model Editor: ' + os.path.basename(xmlFile))
    def save(self):
        if self.srcModel.filename:
            self.srcModel.writeTo()
        else:
            self.saveAs()
    def saveAs(self):
        xmlFile = SaveFileDialog(self).go(pattern='*.xml')
        self.srcModel.writeTo(xmlFile)
        self.srcModel.filename = xmlFile
        self.title('Source Model Editor: ' + os.path.basename(xmlFile))
    def addPointSource(self):
        src = funcFactory.PtSrc(self.ptsrcs)
        self.srcModel[src.name] = src
        self.modelEditor.fill()
        self.currentSource.set(src.name)
        self.modelEditor.selectSource()
        self.ptsrcs += 1
    def addDiffuseSource(self):
        src = funcFactory.DiffuseSrc(self.difsrcs)
        self.srcModel[src.name] = src
        self.modelEditor.fill()
        self.currentSource.set(src.name)
        self.modelEditor.selectSource()
        self.difsrcs += 1
    def deleteSource(self):
        try:
            indx = self.modelEditor.listBox.curselection()
            self.modelEditor.listBox.selection_set(indx)
            srcName = self.modelEditor.listBox.get('active')
            del self.srcModel[srcName]
            self.modelEditor.fill()
            srcName = self.modelEditor.listBox.get('active')
            self.currentSource.set(srcName)
            self.modelEditor.selectSource()
        except:
            pass

class MenuBar(Tk.Menu):
    def __init__(self, parent):
        Tk.Menu.__init__(self)
        self.add_cascade(label="File", menu=FileMenu(parent),
                         underline=0)
        self.add_cascade(label="Edit", menu=EditMenu(parent),
                         underline=0)
        parent.config(menu=self)

class FileMenu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0, postcommand=self.stat)
        self.root = root
        self.add_command(label="Open...", underline=0, command=root.open)
        self.add_command(label="Save", underline=0, command=root.save)
        self.add_command(label="Save as...", command=root.saveAs)
        self.add_command(label="Quit", command=root.quit, underline=0)
    def stat(self):
        if root.srcModel == None:
            self.entryconfigure(1, state=Tk.DISABLED)
            self.entryconfigure(2, state=Tk.DISABLED)
        else:
            self.entryconfigure(1, state=Tk.NORMAL)
            self.entryconfigure(2, state=Tk.NORMAL)

class EditMenu(Tk.Menu):
    def __init__(self, root):
        Tk.Menu.__init__(self, tearoff=0)
        self.add_command(label="Add point source", underline=0,
                         command=root.addPointSource)
        self.add_command(label="Add diffuse source", underline=0,
                         command=root.addDiffuseSource)
        self.add_command(label="Delete selected", underline=0,
                         command=root.deleteSource)

class ModelEditor(Tk.Frame):
    def __init__(self, parent):
        Tk.Frame.__init__(self)
        self.parent = parent
        self.yScroll = Tk.Scrollbar(self, orient=Tk.VERTICAL)
        self.yScroll.pack(side=Tk.RIGHT, fill=Tk.Y)
        self.listBox = Tk.Listbox(self, selectmode=Tk.BROWSE,
                                  yscrollcommand=self.yScroll.set,
                                  height=16)
        self.listBox.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)
        btags = self.listBox.bindtags()
        self.listBox.bindtags(btags[1:] + btags[:1])
        self.listBox.bind('<ButtonRelease-1>', self.selectSource)
        self.yScroll["command"] = self.listBox.yview
        if parent.srcModel:
            self.fill()
    def fill(self):
        self.listBox.delete(0, Tk.END)
        srcModel = self.parent.srcModel
        for srcName in srcModel.names():
            if srcModel[srcName].type == 'PointSource':
                self.listBox.insert(Tk.END, srcName)
            else:
                self.listBox.insert(0, srcName)
    def selectSource(self, event=None):
        if event != None:
            srcName = self.listBox.get('active')
            self.parent.currentSource.set(srcName)
        else:
            srcName = self.parent.currentSource.get()
        try:
            self.parent.importSrcComponents(srcName)
            src = self.parent.srcModel[srcName]
            self.parent.spectrum.setParamList(src.spectrum)
            self.parent.spatialModel.setParamList(src.spatialModel)
            self.currentSrcName = srcName
        except (KeyError, TypeError):
            pass

class ComponentEditor(Tk.Frame):
    def __init__(self, parent, functionDict, title):
        self.parent = parent
        self.functionDict = functionDict
        self.title = title
        Tk.Frame.__init__(self)
        
        initialSrc = functionDict.keys()[0]
        self.currentFunc = Tk.StringVar()
        self.currentFunc.set(self.functionDict[initialSrc].type)
        
        modelFrame = Tk.Frame(self.parent)
        modelFrame.pack(side=Tk.TOP, fill=Tk.X)
        label = Tk.Label(modelFrame, text=title + ": ")
        label.pack(side=Tk.LEFT, fill=Tk.X)
        self.functionMenu = Tk.Menubutton(modelFrame,
                                          text=functionDict[initialSrc].type,
                                          relief=Tk.RAISED, bd=1)
        self.functionMenu.pack(side=Tk.LEFT, fill=Tk.X)
        self.functionMenu["menu"] = FunctionMenu(self.functionMenu, self,
                                                 functionDict)

        paramFrame = Tk.Frame(self.parent)
        paramFrame.pack(side=Tk.BOTTOM, fill=Tk.BOTH)
        self.yScroll = Tk.Scrollbar(paramFrame, orient=Tk.VERTICAL)
        self.yScroll.pack(side=Tk.RIGHT, fill=Tk.Y)
        self.paramListBox = Tk.Listbox(paramFrame, selectmode=Tk.BROWSE,
                                       yscrollcommand=self.yScroll.set,
                                       height=3, width=30)
        self.paramListBox.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=Tk.YES)
        btags = self.paramListBox.bindtags()
        self.paramListBox.bindtags(btags[1:] + btags[:1])
        self.paramListBox.bind('<Double-ButtonRelease-1>', self.attrDialog)
        self.yScroll["command"] = self.paramListBox.yview
    def setParamList(self, srcComponent):
        self.srcComponent = srcComponent
        self.paramListBox.delete(0, Tk.END)
        for paramName in srcComponent.parameters:
            param = srcComponent.parameters[paramName]
            entry = '%s: %.2f' % (paramName, param.value)
            self.paramListBox.insert(Tk.END, entry)
        self.functionMenu.config(text=srcComponent.type)
        self.currentFunc.set(srcComponent.type)
    def attrDialog(self, event):
        paramName = self.paramListBox.get('active').split(':')[0]
        myDialog = AttributeDialog(self.parent, self.srcComponent, paramName)
        self.setParamList(self.srcComponent)

class AttributeDialog(mySimpleDialog.Dialog):
    def __init__(self, parent, func, paramName):
        self.parent = parent
        self.func = func
        self.paramName = paramName
        mySimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        dataFrame = Tk.Frame(self)
        dataFrame.pack(anchor=Tk.N, expand=Tk.YES, fill=Tk.X)
        self.title(self.func.type + ": " + self.paramName)
        self.addSubFrames(dataFrame)
    def addSubFrames(self, parent):
        self.attributes = []
        paramDict = self.func.parameters[self.paramName].__dict__
        self.parameter = self.func.parameters[self.paramName]
        self.paramNode = self.parameter.node
        row = 0
        for attributeName in paramDict.keys():
            if attributeName != 'node' and attributeName != 'name':
                name = attributeName.encode()
                value = self.paramNode.getAttribute(attributeName).encode()
                self.attributes.append(AttributeEntry(parent, name,
                                                      value, row))
                row += 1
    def apply(self):
        for attribute in self.attributes:
            try:
                if attribute.name == 'free':
                    value = int(string.atof(attribute.variable.get()))
                    if value != 0:
                        value = 1
                else:
                    value = string.atof(attribute.variable.get())
                    self.parameter.__dict__[attribute.name] = value
                    self.parameter.setAttributes()
            except:
                pass
            
class AttributeEntry:
    def __init__(self, parent, name, value, row):
        self.parent = parent
        self.name = name
        self.variable = Tk.StringVar()
        self.variable.set(value)
        self.makeLabel(row)
        entry = Tk.Entry(self.parent, textvariable=self.variable,
                         width=30, state=Tk.NORMAL)
        entry.grid(row=row, column=1)
    def makeLabel(self, row):
        name = Tk.Label(self.parent, text=self.name)
        name.grid(row=row, column=0)
    def value(self):
        try:
            my_value = "%s" % eval(self.variable.get())
        except:
            my_value = self.variable.get()
        return my_value
        
class FunctionMenu(Tk.Menu):
    def __init__(self, parent, root, functionDict):
        self.parent = parent
        self.root = root
        self.functionDict = functionDict
        self.buttonName = functionDict[functionDict.keys()[0]].type
        Tk.Menu.__init__(self, self.parent, tearoff=0)
        for function in functionDict.keys():
            self.add_command(label=functionDict[function].type,
                             command=self.SetName(self, self.root,
                                                  functionDict[function].type))
    class SetName(object):
        def __init__(self, parent, root, name):
            self.parent = parent
            self.root = root
            self.name = name
        def __call__(self):
            self.root.currentFunc.set(self.name)
            self.root.paramListBox.delete(0, self.root.paramListBox.size()-1)
            func = self.root.functionDict[self.name]
            self.root.setParamList(func)
            self.parent.buttonName = self.name
            self.parent.parent.config(text=self.name)

class FunctionItem(object):
    def __init__(self, name, params):
        self.name = name
        self.parameters = params

spectralFuncs = funcFactory.Spectra()
spatialFuncs = funcFactory.SpatialModels()

if __name__ == "__main__":
    root = RootWindow(spectralFuncs, spatialFuncs)
    root.mainloop()
