#!/usr/bin/env python
"""
Dialog interface to a .par file.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""

import os, sys
import Tkinter
import mySimpleDialog
from FileDialog import LoadFileDialog, SaveFileDialog
from EditFileDialog import EditFileDialog
from pfilesPath import pfilesPath
import random

class ParamDialog(mySimpleDialog.Dialog):
    def __init__(self, parent, file):
        self.parent = parent
        self.file = os.path.join(pfilesPath(file), file)
        self.paramString = None
        mySimpleDialog.Dialog.__init__(self, parent)
    def body(self, parent):
        dataFrame = Tkinter.Frame(self)
        dataFrame.pack(anchor=Tkinter.N, expand=Tkinter.YES, fill=Tkinter.X)
        self.title(self.file)
        self.addSubFrames(dataFrame)
    def addSubFrames(self, parent):
        try:
            file = open(self.file, "r")
        except:
            self.destroy()
            raise
        lines = file.readlines()
        self.parameters = []
        row = 0
        for line in lines:
            if line.find("#") != 0 and len(line.strip()):
                self.parameters.append(ParamEntry(parent,
                                                  line.split(','), row))
                row += 1
    def apply(self):
        self.paramString = ""
        for param in self.parameters:
            self.paramString += (" " + param.value())

class ParamEntry:
    def __init__(self, parent, fields, row):
        self.fields = [field.strip(" ") for field in fields]
        self.parent = parent
        self.variable = Tkinter.StringVar()
        if os.name == "nt" and self.fields[1].find("f") != -1:
            self.fields[3] = filter_backslash(self.fields[3])
        self.variable.set(self.fields[3].strip('"'))
        if self.fields[1].find("f") != -1:
            self.makeFileDialog(row)
            entryState = Tkinter.NORMAL
        elif self.fields[1] == "b":
            self.makeBoolMenu(row)
            entryState = Tkinter.DISABLED
        elif self.fields[4].find("|") > 0:
            self.makeOptionsMenu(row)
            entryState = Tkinter.DISABLED
        elif self.fields[0] == "random_seed":
            self.makeRandomSeed(row)
            entryState = Tkinter.NORMAL
        else:
            self.makeLabel(row)
            entryState = Tkinter.NORMAL
        entry = Tkinter.Entry(self.parent, textvariable=self.variable,
                              width=60, state=entryState)
        entry.grid(column=1, row=row)
    def makeFileDialog(self, row):
        w = Tkinter.Button(self.parent, text=self.fields[0],
                           command=lambda:self.getFile(self.fields[0]), bd=1)
        w.grid(column=0, row=row, sticky=Tkinter.E)
    def makeBoolMenu(self, row):
        menuButton = Tkinter.Menubutton(self.parent, text=self.fields[0], 
                                        cursor="hand2", underline=0,
                                        relief=Tkinter.RAISED, bd=1)
        menuButton.grid(column=0, row=row, sticky=Tkinter.E)
        menu = Tkinter.Menu(menuButton, tearoff=0)
        menu.add_command(label="yes", command=SetVar(self, "yes"))
        menu.add_command(label="no", command=SetVar(self, "no"))
        menuButton["menu"] = menu
    def makeOptionsMenu(self, row):
        options = self.fields[4].split("|")
        menuButton = Tkinter.Menubutton(self.parent, text=self.fields[0], 
                                        cursor="hand2", underline=0,
                                        relief=Tkinter.RAISED, bd=1)
        menuButton.grid(column=0, row=row, sticky=Tkinter.E)
        menu = Tkinter.Menu(menuButton, tearoff=0)
        for option in options:
            menu.add_command(label=option, command=SetVar(self, option))
        menuButton["menu"] = menu
    def makeRandomSeed(self, row):
        """Special button to create a random seed for the CLHEP generator
           used by obsSim"""
        w = Tkinter.Button(self.parent, text=self.fields[0],
                           command=self.getRandomInt, bd=1)
        w.grid(column=0, row=row, sticky=Tkinter.E)
    def makeLabel(self, row):
        name = Tkinter.Label(self.parent, text = self.fields[0])
        name.grid(column=0, row=row, sticky=Tkinter.E)
    def value(self):
        try:
            my_value = "%s" % eval(self.variable.get())
        except:
            my_value = self.variable.get()
        return my_value
    def getFile(self, label='Edit File Dialog'):
        dialog = EditFileDialog(self.parent, label)
        file = dialog.go(self.variable.get())
        if file:
            try:
                if os.path.samefile(file, os.path.basename(file)):
                    file = os.path.basename(file)
            except OSError:
                pass
            self.variable.set(file)
    def getRandomInt(self):
        my_int = int(random.random()*sys.maxint/1e3)
        self.variable.set(`my_int`)
        
class SetVar:
    def __init__(self, parent, value):
        self.parent = parent
        self.value = value
    def __call__(self):
        try:
            self.value = eval(self.value)
        except:
            pass
        self.parent.variable.set(self.value)

def filter_backslash(path):
    #
    # PIL adds backslashes to Windows paths, so we need to filter them
    # out again.
    #
    while path.find(r"\\") > 0:
        path = path.replace(r"\\", "\\")
    return path

if __name__ == "__main__":
    root = Tkinter.Tk()
    dialog = ParamDialog(root, sys.argv[1])
    root.mainloop()
    print dialog.paramString
