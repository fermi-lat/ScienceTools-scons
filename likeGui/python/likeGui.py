#!/usr/bin/env python
"""
Prototype GUI interface for the likelihood program.

@authors J. Chiang <jchiang@slac.stanford.edu>
         P. Nolan <pln@razzle.stanford.edu>
"""
#
#$Header$
#

import sys, os, time, string
import Tkinter
import mySimpleDialog
import tkSimpleDialog, tkFileDialog
import tkFont
from FileDialog import LoadFileDialog, SaveFileDialog, FileDialog
from EditFileDialog import *
from ScrolledText import ScrolledText
from ThreadedClient import *
import readXml
import extractSources

likePath = os.path.join(os.environ['LIKELIHOODROOT'], os.environ['BINDIR'], '')
                        
likeProgram = likePath + "likelihood.exe"
likeProgram = cleanPathName(likeProgram)
TsMapProgram = likePath + "TsMap.exe"
expMapProgram = likePath + "expMap.exe"
expCubeProgram = likePath + "makeExposureCube.exe"
diffRespProgram = likePath + "diffuseResponses.exe"
obsSimProgram = os.path.join(os.environ['OBSERVATIONSIMROOT'],
                             os.environ['BINDIR'],
                             'obsSim.exe')
dataSubselectorProgram = os.path.join(os.environ['DATASUBSELECTORROOT'],
                                      os.environ['BINDIR'],
                                      'dataSubselector.exe')
evtbin = os.path.join(os.environ['EVTBINROOT'], os.environ['BINDIR'],
                      'evtbin.exe')
rspgen = os.path.join(os.environ['RSPGENROOT'], os.environ['BINDIR'],
                      'rspgen.exe')

class RootWindow(Tkinter.Tk):
    def __init__(self, executable=likeProgram, debug=0):
        Tkinter.Tk.__init__(self)
        self.executable = executable
        self.debug = debug
        self.title("likeGui: " + os.path.abspath(''))
        self.menuBar = MenuBar(self)
        self.textPanel = TextPanel(self)
        self.likeThread = None
        self.threads = {}
        self.collectOutput()
    def openDialog(self):
        SourceDialog(self)
    def openList(self):
        SourceList(self)
    def writeText(self, text):
        self.textPanel.writeText(text)
    def enableEditing(self):
        self.textPanel.enableEditing()
    def disableEditing(self):
        self.textPanel.disableEditing()
    def myQuit(self):
        self.stop()
        time.sleep(0.1)
        sys.exit()
    def run(self):
        if not self.likeThread or self.likeThread.ended:
            try:
                self.likeThread = likelihoodClient(self, likeProgram,
                                                   debug=self.debug)
            except IOError:
                self.writeText('likelihood.par not found.\n')
                return
            self.threads['likelihood'] = self.likeThread
        else:
            self.likeThread.sendMessage("y\n")
    def addThread(self, thread):
        label = "thread%i" % len(self.threads)
        self.threads[label] = thread
    def collectOutput(self):
        "Poll the ThreadedClient objects using the Tkinter.Tk.after method"
        for thread in self.threads.keys():
            self.flushOutputQueue(self.threads[thread].outputQueue)
        self.after(200, self.collectOutput)
    def flushOutputQueue(self, queue):
        while queue.qsize():
            try:
                message = queue.get(0)
                self.writeText(message)
            except Queue.Empty:
                pass
    def stop(self):
        if self.likeThread:
            self.likeThread.ended = 1
            self.likeThread.sendMessage("q\n")
            del self.threads['likelihood']
            self.flushOutputQueue(self.likeThread.outputQueue)
            self.likeThread = None
            self.writeText(os.path.basename(self.executable)
                           + " execution terminated.\n")
    def saveLog(self):
        filename = SaveFileDialog(self).go()
        if filename:
            self.textPanel.save(filename)

class MenuBar(Tkinter.Menu):
    def __init__(self, parent):
        self.parent = parent
        Tkinter.Menu.__init__(self)
        self.add_cascade(label="File", menu=FileMenu(self, parent),
                         underline=0)
        self.add_cascade(label="Likelihood", menu=LikeMenu(self, parent),
                         underline=0)
        self.add_cascade(label="Helper Apps", menu=AppsMenu(self, parent),
                         underline=0)
        if self.parent.debug:
            self.add_cascade(label="Debug", menu=DebugMenu(self, parent),
                             underline=0)
        parent.config(menu=self)

class FileMenu(Tkinter.Menu):
    def __init__(self, parent, grandparent):
        self.parent = parent
        self.grandparent = grandparent
        Tkinter.Menu.__init__(self, tearoff=0, postcommand=self.stat)
        self.add_command(label='Execute a Python command...',
                         command=self.sendPythonCommand, underline=10) 
        self.add_separator()
        self.add_command(label='cd...', command=self.cd, underline=0)
        self.add_command(label="Edit file...", command=self.editFile,
                         underline=0)
        self.add_command(label="Save log as...",
                         command=self.grandparent.saveLog, underline=0)
        self.add_command(label="Enable text panel editing",
                         command=self.grandparent.enableEditing,
                         underline=1)
        self.add_command(label="Quit", command=self.grandparent.myQuit,
                         underline=0)
    def stat(self):
        if self.grandparent.textPanel.defaultState == Tkinter.DISABLED:
            self.entryconfigure(5, label="Enable text panel editing", 
                                command=self.grandparent.enableEditing,
                                underline=1)
        else:
            self.entryconfigure(5, label="Disable text panel editing", 
                                command=self.grandparent.disableEditing,
                                underline=0)
    def cd(self):
        file = tkFileDialog.askdirectory(title="Where to?", mustexist=1)
        #file = FileDialog(self.parent).go()
        try:
            os.chdir(file)
        except:
            pass
        self.grandparent.writeText(os.path.abspath(os.curdir) + "\n")
    def editFile(self):
        dialog = EditFileDialog(self.parent)
        dialog.editVar.set(1)
        filename = dialog.go()
    def sendPythonCommand(self):
        cmd = tkSimpleDialog.askstring("Python command",
                                       "Enter a command to send " + 
                                       "to the Python interpreter")
        try:
            import cStringIO
            save = sys.stdout
            sys.stdout = cStringIO.StringIO()
            exec(cmd)
            result = sys.stdout.getvalue()
            sys.stdout = save
            self.grandparent.writeText(result)
        except:
            try:
                self.grandparent.writeText("Sorry, " + cmd + " failed.\n")
            except:
                pass

class LikeMenu(Tkinter.Menu):
    def __init__(self, parent, grandparent):
        self.parent = parent
        self.grandparent = grandparent
        Tkinter.Menu.__init__(self, tearoff=0, postcommand=self.stat)
        self.add_command(label="Create a source model...",
                         command=self.extract, underline=0)
        self.add_command(label="Modify/View a source model...",
                         command=self.openFile, underline=0)
        self.add_command(label="Run likelihood...",
                         command=self.grandparent.run, underline=0)
        self.add_command(label="Stop", command=self.grandparent.stop,
                         underline=0)
    def stat(self):
        if (not self.grandparent.likeThread
            or self.grandparent.likeThread.ended):
            self.entryconfigure(2, label="Run likelihood...")
        else:
            self.entryconfigure(2, label="Refit")
    def openFile(self):
        xmlFile = EditFileDialog(self.grandparent).go(pattern='*.xml')
        self.readFile(xmlFile)
    def readFile(self, xmlFile):
        try:
            srcModel = readXml.SourceModel(xmlFile)
            if len(srcModel.names()):
                self.grandparent.srcModel = srcModel
                self.grandparent.srcModel.filename = xmlFile
                self.grandparent.openList()
            else:
                self.grandparent.writeText("No Sources found in "
                                           + xmlFile + "\n")
        except:
            if xmlFile:
                self.grandparent.writeText("Error reading " + xmlFile + "\n")
    def extract(self):
        sources = extractSources.SourceRegionDialog(self.grandparent)
        if sources.haveSources:
            self.readFile(sources.filename.value())

class AppsMenu(Tkinter.Menu):
    def __init__(self, parent, grandparent):
        self.parent = parent
        self.grandparent = grandparent
        self.debug = grandparent.debug
        Tkinter.Menu.__init__(self, tearoff=0)
        self.add_command(label="makeExposureCube",
                         command=self.expCube, underline=0)
        self.add_command(label="expMap", command=self.expMap, underline=1)
        self.add_command(label="TsMap", command=self.TsMap, underline=0)
        self.add_command(label="diffuseResponses",
                         command=self.diffuseResponses,
                         underline=7)
        self.add_separator()
        self.add_command(label="obsSim", command=self.obsSim, underline=3)
        self.add_separator()
        self.add_command(label="dataSubselector", command=self.dataSubselector,
                         underline=0)
        self.add_separator()
        self.add_command(label="evtbin", command=self.evtbin, underline=3)
        self.add_command(label="rspgen", command=self.rspgen, underline=0)
        self.add_separator()
        self.add_command(label="ds9 (image)", command=self.ds9, underline=5)
        self.add_command(label="ds9 (events)", command=lambda:self.ds9(1),
                         underline=5)
    def expCube(self):
        try:
            myThread = likelihoodClient(self.grandparent, expCubeProgram,
                                        'makeExposureCube.par', useChars = 1,
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('makeExposureCube.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def expMap(self):
        try:
            myThread = likelihoodClient(self.grandparent, expMapProgram,
                                        'expMap.par', useChars = 1,
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('expMap.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def TsMap(self):
        try:
            myThread = likelihoodClient(self.grandparent, TsMapProgram,
                                        'TsMap.par', useChars = 1,
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('TsMap.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def diffuseResponses(self):
        try:
            myThread = likelihoodClient(self.grandparent, diffRespProgram,
                                        'diffuseResponses.par', useChars = 1,
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('diffuseResponses.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def obsSim(self):
        try:
            myThread = likelihoodClient(self.grandparent, obsSimProgram,
                                        'obsSim.par', useChars = 1,
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('obsSim.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def dataSubselector(self):
        try:
            myThread = likelihoodClient(self.grandparent,
                                        dataSubselectorProgram,
                                        'dataSubselector.par',
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('dataSubselector.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def evtbin(self):
        try:
            myThread = likelihoodClient(self.grandparent, evtbin, 'evtbin.par',
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('evtbin.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def rspgen(self):
        try:
            myThread = likelihoodClient(self.grandparent, rspgen, 'rspgen.par',
                                        debug=self.debug)
        except IOError:
            self.grandparent.writeText('rspgen.par not found.\n')
            return
        self.grandparent.addThread(myThread)
    def ds9(self, events=0):
        file = EditFileDialog(self.grandparent).go(pattern='*.fits')
        if file:
            if events:
                command = 'ds9 "' + file + '[1][bin=ra,dec]"'
            else:
                command = 'ds9 ' + file
            runInThread(self.popenApp(self.grandparent, command))
    class popenApp:
        """
        A special currying class for wrapping command line strings
        sent to os.popen while providing access to the master widget's
        writeText method.
        """
        def __init__(self, master, command):
            self.master = master
            self.command = command
        def __call__(self):
            if os.name == "posix":
                output = os.popen(self.command)
            else:
                import win32pipe
                output = win32pipe.popen(self.command, 'r')
            lines = output.readlines()
            for line in lines:
                self.master.writeText(line)

class DebugMenu(Tkinter.Menu):
    def __init__(self, parent, grandparent):
        self.parent = parent
        self.grandparent = grandparent
        Tkinter.Menu.__init__(self, tearoff=0)
        self.add_command(label='List threads...', command=self.listThreads)
        self.add_command(label='Delete threads...', command=self.deleteThreads)
    def writeText(self, text):
        self.grandparent.writeText(text)
    def listThreads(self):
        for name in self.grandparent.threads.keys():
            self.writeText("%s: %s\n" % (name, self.grandparent.threads[name]))
    def deleteThreads(self):
        self.writeText("Deleting all non-likelihood threads...\n")
        for name in self.grandparent.threads.keys():
            if name != 'likelihood':
                try:
                    del self.grandparent.threads[name]
                except:
                    pass

class TextPanel(ScrolledText):
    def __init__(self, parent):
        self.defaultState = Tkinter.DISABLED
        ScrolledText.__init__(self, relief=Tkinter.SUNKEN, 
                              bd=2, width=80, height=25,
                              bg="white", fg="black",
                              state=self.defaultState,
                              font=tkFont.Font(font="lucidatypewriter 12"),
                              insertofftime=0, insertontime=0)
        self.pack(side=Tkinter.TOP, expand=Tkinter.YES,
                  fill=Tkinter.BOTH)
        self.text = ""
    def writeText(self, text):
        self.text += text
        self.config(state=Tkinter.NORMAL)
        self.insert(Tkinter.END, text)
        self.update_idletasks()
        self.see(Tkinter.END)
        self.config(state=self.defaultState)
    def save(self, filename):
        file = open(filename, "w")
        file.write(self.text)
        file.close()
    def enableEditing(self):
        self.config(state=Tkinter.NORMAL)
        self.defaultState = Tkinter.NORMAL
    def disableEditing(self):
        self.config(state=Tkinter.DISABLED)
        self.defaultState = Tkinter.DISABLED

class SourceList(Tkinter.Tk):
    def __init__(self, parent):
        self.srcModel = parent.srcModel
        self.parent = parent
        Tkinter.Tk.__init__(self)
        self.yScroll = Tkinter.Scrollbar(self, orient=Tkinter.VERTICAL)
        self.yScroll.grid(row=0, column=1, sticky=Tkinter.N + Tkinter.S)
        self.listBox = Tkinter.Listbox(self, selectmode=Tkinter.MULTIPLE,
                                       yscrollcommand=self.yScroll.set)
        self.listBox.grid(row=0, column=0,
                          sticky=Tkinter.N+Tkinter.S+Tkinter.E+Tkinter.W)
        self.yScroll["command"] = self.listBox.yview
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self.buttons(self)
        self.title(self.srcModel.filename)
        self.fill()
        self.selectAll()
    def buttons(self, parent):
        box = Tkinter.Frame(parent)
        w = Tkinter.Button(box, text="Select All", command=self.selectAll)
        w.pack(side=Tkinter.LEFT, padx=5, pady=4, fill=Tkinter.X, expand=1)
        w = Tkinter.Button(box, text="Clear All", command=self.clearAll)
        w.pack(side=Tkinter.LEFT, padx=5, pady=4, fill=Tkinter.X, expand=1)
        w = Tkinter.Button(box, text="Save Selected",
                           command=self.saveSelected)
        w.pack(side=Tkinter.LEFT, padx=5, pady=4, fill=Tkinter.X, expand=1)
        w = Tkinter.Button(box, text="View/Modify Selected...",
                           command=self.openDialog)
        w.pack(side=Tkinter.LEFT, padx=5, pady=4, fill=Tkinter.X, expand=1)
        w = Tkinter.Button(box, text="Close", command=self.destroy)
        w.pack(side=Tkinter.LEFT, padx=5, pady=4, fill=Tkinter.X, expand=1)
        box.grid(row=1, column=0, sticky="EW")
        #self.rowconfigure(1, weight=1)
    def selectAll(self):
        self.listBox.selection_set(0, self.listBox.size())
    def clearAll(self):
        self.listBox.selection_clear(0, self.listBox.size())
    def saveSelected(self):
        self.setShowFlags()
        for name in self.srcModel.names():
            if self.srcModel[name].show == 0:
                del self.srcModel[name]
        self.srcModel.writeTo()
        self.listBox.delete(0, self.listBox.size()-1)
        self.fill()
        self.listBox.selection_set(0, self.listBox.size()-1)
    def openDialog(self):
        self.setShowFlags()
        self.destroy()
        self.parent.openDialog()
    def addLine(self, line):
        self.listBox.insert(Tkinter.END, line)
    def setShowFlags(self):
        self.resetShowFlags()
        lines = self.listBox.curselection()
        for indx in lines:
            self.srcModel[self.names[string.atoi(indx)]].show = 1
    def resetShowFlags(self, value=0):
        for name in self.srcModel.names():
            self.srcModel[name].show = value
    def fill(self):
        self.names = []
        for srcName in self.srcModel.names():
            if self.srcModel[srcName].type != "PointSource":
                self.addLine(self.srcModel[srcName].summary())
                self.names.append(srcName)
        for srcName in self.srcModel.names():
            if self.srcModel[srcName].type == "PointSource":
                self.addLine(self.srcModel[srcName].summary())
                self.names.append(srcName)

class SourceDialog(mySimpleDialog.Dialog):
    def __init__(self, parent):
        self.srcModel = parent.srcModel
        self.parent = parent
        mySimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        self.load()
    def buttonbox(self):
        box = Tkinter.Frame(self)
        w = Tkinter.Button(box, text="OK", command=self.ok)
        w.pack(side=Tkinter.LEFT, padx=5, pady=5)
        w = Tkinter.Button(box, text="Save and Run", command=self.save)
        w.pack(side=Tkinter.LEFT, padx=5, pady=5)
        w = Tkinter.Button(box, text="Save as...", command=self.saveAs)
        w.pack(side=Tkinter.LEFT, padx=5, pady=5)
        w = Tkinter.Button(box, text="Reload", command=self.reload)
        w.pack(side=Tkinter.LEFT, padx=5, pady=5)
        w = Tkinter.Button(box, text="Cancel", command=self.cancel)
        w.pack(side=Tkinter.LEFT, padx=5, pady=5)
        box.pack()
        self.commandFrame = box
    def addSubFrames(self, f):
        f.srcName = Tkinter.Frame(f)
        f.srcName.pack(side=Tkinter.LEFT)
        f.srcPos = Tkinter.Frame(f)
        f.srcPos.pack(side=Tkinter.LEFT)
        f.srcSpectrum = Tkinter.Frame(f)
        f.srcSpectrum.pack(side=Tkinter.RIGHT)
        f.row = 1
        self.sources = []
        for srcName in self.srcModel.names():
            if (self.srcModel[srcName].type == "PointSource"
                and self.srcModel[srcName].show):
                self.sources.append(SourceEntry(f, self.srcModel[srcName]))
                f.row += 1
        for srcName in self.srcModel.names():
            if (self.srcModel[srcName].type != "PointSource"
                and self.srcModel[srcName].show):
                self.sources.append(SourceEntry(f, self.srcModel[srcName]))
                f.row += 1
    def apply(self):
        self.save()
    def save(self):
        self.parent.writeText("Writing source model to "
                              + self.parent.srcModel.filename + "\n")
        for src in self.sources:
            src.apply()
        self.parent.srcModel.writeTo()
        self.parent.run()
        #time.sleep(0.1)
        #self.reload(quiet=0)
    def saveAs(self):
        file = SaveFileDialog(self.parent).go()
        if file:
            self.parent.srcModel.filename = file
            self.save()
            self.reload(quiet=1)
    def reload(self, quiet=0):
        if not quiet:
            self.parent.writeText("Re-reading source model from "
                                  + self.parent.srcModel.filename + "\n")
        self.dataFrame.destroy()
        self.commandFrame.destroy()
        xmlFile = self.parent.srcModel.filename
        self.parent.srcModel = readXml.SourceModel(xmlFile)
        self.srcModel = self.parent.srcModel
        self.load()
        self.buttonbox()
    def load(self):
        self.dataFrame = Tkinter.Frame(self)
        self.dataFrame.pack(anchor=Tkinter.N, expand=Tkinter.YES,
                            fill=Tkinter.X)
        self.title(self.srcModel.filename)
        self.addSubFrames(self.dataFrame)

class SourceEntry:
    def __init__(self, parent, srcElement):
        self.parent = parent
        self.srcElement = srcElement
        self.nameVar = Tkinter.StringVar()
        self.nameVar.set(srcElement.name)
        nameEntry = Tkinter.Entry(parent.srcName, textvariable=self.nameVar)
        nameEntry.grid(column=1, row=parent.row, sticky=Tkinter.W)
        if (srcElement.type == "PointSource"):
            self.addRaDec()
        else:
            nullEntry = Tkinter.Label(parent.srcPos, text=" ")
            nullEntry.grid(column=0, row=parent.row)
        self.spectrum = {}
        self.col = 0
        for paramName in srcElement.spectrum.parameters.keys():
            if paramName != "Scale":
                self.addParam(paramName)
    def addRaDec(self):
        self.raVar = Tkinter.DoubleVar()
        self.raVar.set(self.srcElement.spatialModel.RA.value)
        raLabel = Tkinter.Label(self.parent.srcPos, text = "RA")
        raLabel.grid(column=0, row=self.parent.row)
        ra = Tkinter.Entry(self.parent.srcPos, textvariable=self.raVar,
                           width=10)
        ra.grid(column=1, row=self.parent.row, sticky=Tkinter.E)
        self.decVar = Tkinter.DoubleVar()
        self.decVar.set(self.srcElement.spatialModel.DEC.value)
        decLabel = Tkinter.Label(self.parent.srcPos, text = "Dec")
        decLabel.grid(column=2, row=self.parent.row)
        dec = Tkinter.Entry(self.parent.srcPos, textvariable=self.decVar,
                            width=10)
        dec.grid(column=3, row=self.parent.row, sticky=Tkinter.E)
    def addParam(self, paramName):
        self.spectrum[paramName] = (Tkinter.DoubleVar(), Tkinter.BooleanVar())
        value = self.spectrum[paramName][0]
        value.set(self.srcElement.spectrum.parameters[paramName].value)
        paramLabel = Tkinter.Label(self.parent.srcSpectrum, text = paramName)
        paramLabel.grid(column=self.col, row=self.parent.row)
        self.col += 1
        paramValue = Tkinter.Entry(self.parent.srcSpectrum,
                                   textvariable = value, width = 10)
        paramValue.grid(column=self.col, row=self.parent.row)
        self.col += 1
        freeFlag = self.spectrum[paramName][1]
        freeValue = self.srcElement.spectrum.parameters[paramName].free
        freeFlag.set(bool(freeValue))
        button = Tkinter.Checkbutton(self.parent.srcSpectrum,
                                     variable = freeFlag)
        button.grid(column=self.col, row=self.parent.row)
        self.col += 1
    def apply(self):
        self.srcElement.name = self.nameVar.get()
        if (self.srcElement.type == "PointSource"):
            self.srcElement.spatialModel.RA.value = float(self.raVar.get())
            self.srcElement.spatialModel.DEC.value = float(self.decVar.get())
        for paramName in self.spectrum.keys():
            value = float(self.spectrum[paramName][0].get())
            self.srcElement.spectrum.parameters[paramName].value = value
            free = int(self.spectrum[paramName][1].get())
            self.srcElement.spectrum.parameters[paramName].free = free

if __name__ == "__main__":
    debug = 0
    for name in sys.argv:
        if name == "-debug":
            debug = 1
    root = RootWindow(debug=debug)
    root.mainloop()
