"""
Classes for wrapping FTOOLs as threaded clients run from a master
Tkinter widget.

@authors J. Chiang <jchiang@slac.stanford.edu>
         P. Nolan <pln@razzle.stanford.edu>

$Header$
"""

import os, re
import threading
import Queue, time
from ParamDialog import ParamDialog

def runInThread(app):
    myThread = threading.Thread(target=app)
    myThread.start()

class ThreadedClient:
    def __init__(self, master, executable, pfile, useChars = 0, debug=0):
        self.master = master
        self.executable = executable
        self.useChars = useChars
        self.ended = 0
        self.outputQueue = Queue.Queue()
        self.inputQueue = Queue.Queue()
        self.outputFile, self.inputFile = None, None
        try:
            self.dialog = ParamDialog(self.master, pfile)
            if self.dialog.paramString:
                if debug:
                    appName = executable
                else:
                    appName = os.path.basename(executable)
                self.outputQueue.put("Launching " + appName + "...\n")
                runInThread(self.runApp)
            else:
                self.ended = 1
        except 'ParFileError', message:
            self.master.writeText(message + '\n')
    def runApp(self):
        command = self.executable + self.dialog.paramString
        if os.name == 'posix':
            import popen2
            (self.inputFile, self.outputFile) = popen2.popen4(command, 0)
        else:
            import win32pipe
            (self.outputFile, self.inputFile) = win32pipe.popen4(command)
        runInThread(self.grabText)
    def handleMessages(self):
        if not self.inputQueue: return
        while 1:
            cmd = self.inputQueue.get()
            try:
                self.outputFile.write(cmd)
            except: pass
            if self.inputQueue.empty(): break
    def grabText(self):
        if self.inputFile:
            if self.useChars:
                self.processChars()
            else:
                self.processLines()
    def processLines(self):
        while 1:
            line = self.inputFile.readline()
            self.outputQueue.put(line)
    def processChars(self):
        while 1:
            c = self.inputFile.read(1)
            self.outputQueue.put(c)

class likelihoodClient(ThreadedClient):
    def __init__(self, master, executable, pfile = 'likelihood.par',
                 useChars = 0, queryString = "Refit", debug=0):
        self.queryString = queryString
        self.paused = 0
        ThreadedClient.__init__(self, master, executable, pfile, useChars,
                                debug=debug)
    def sendMessage(self, message):
        if self.inputQueue:
            self.inputQueue.put(message)
            runInThread(self.handleMessages)
    def processLines(self):
        line = self.inputFile.readline()
        while line:
            if re.match(self.queryString, line):
                self.paused = 1
            else:
                self.paused = 0
            self.outputQueue.put(line)
            line = self.inputFile.readline()
    def processChars(self):
        c = ' '
        while c:
            c = self.inputFile.read(1)
            self.outputQueue.put(c)
        self.outputQueue.put('\n')

def envVarPath(envVar):
    path = os.getenv(envVar)
    if path:
        path += "/"
    else:
        path = ""
    return path
