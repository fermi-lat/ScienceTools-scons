#!/usr/bin/env python
"""
Allow an option to edit the selected file via an Edit check button,
added to the standard FileDialog.FileDialog.

@author J. Chiang <jchiang@slac.stanford.edu>

$Header$
"""

import Tkinter, os, sys
from FileDialog import FileDialog

class EditFileDialog(FileDialog):
    def __init__(self, master, title='Edit File Dialog'):
        self.master = master
        FileDialog.__init__(self, master, title)
        self.addCheckButton()
    def go(self, dir_or_file=os.curdir, pattern="*", default="", key=None):
        filename = FileDialog.go(self, dir_or_file, pattern, default, key)
        if self.edit() and filename:
            self.launchEditor(filename)
        return cleanPathName(filename)
    def launchEditor(self, file):
        file = cleanPathName(file)
        editor = os.getenv("EDITOR")
        if editor:
            os.system(editor + " " + file)
        else:
            self.catFile(file)
    def catFile(self, file):
        print "Contents of " + file + ":\n"
        f = open(file, "r")
        for line in f.readlines():
            sys.stdout.write(line)
    def addCheckButton(self):
        self.subframe = Tkinter.Frame(self.top)
        self.subframe.pack(side=Tkinter.BOTTOM, fill=Tkinter.X)
        self.editVar = Tkinter.BooleanVar()
        self.editVar.set(0)
        w = Tkinter.Checkbutton(self.subframe, variable=self.editVar,
                                text="Edit")
        w.pack(side=Tkinter.LEFT)
    def edit(self):
        return self.editVar.get()

def cleanPathName(file):
    if not file: return None
    import re
    pat = re.compile("\$\\((.*)\\)(.*)") # expand env. vars
    mat = pat.match(file)
    if mat:
        prefix = os.environ[mat.group(1)]
        file = prefix + os.sep + mat.group(2)
    if os.name == "nt":
        import win32api
        try:
            shortpath = win32api.GetShortPathName(file)
        except:
            shortpath = file
        return shortpath            
    else:
        return file

if __name__ == "__main__":
    root = Tkinter.Tk()
    myLoader = myLoadFile(root)
    filename = myLoader.go()
    print filename, myLoader.edit()

    

