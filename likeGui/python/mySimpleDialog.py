#
# mySimpleDialog.py
#
# Copyright (c) 1997 by Fredrik Lundh
#
# fredrik@pythonware.com
# http://www.pythonware.com
#

# --------------------------------------------------------------------
# dialog base class

'''Dialog boxes

This module handles dialog boxes. It contains the following
public symbols:

Dialog -- a base class for dialogs

   This class has been copied verbatim from tkSimpleDialog.py, and the
   line 'self.transient(parent)' has been commented out, so that the
   dialog window is not slaved to the parent widget.

   J. Chiang <jchiang@slac.stanford.edu>

   $Id$
'''

from Tkinter import *
import os

class Dialog(Toplevel):

    '''Class to open dialogs.

    This class is intended as a base class for custom dialogs
    '''

    def __init__(self, parent, title = None):

        '''Initialize a dialog.

        Arguments:

            parent -- a parent window (the application window)

            title -- the dialog title
        '''
        Toplevel.__init__(self, parent)
#        self.transient(parent)

        if title:
            self.title(title)

        self.parent = parent

        self.result = None

        body = Frame(self)
        self.initial_focus = self.body(body)
        body.pack(padx=5, pady=5)

        self.buttonbox()

        self.grab_set()

        if not self.initial_focus:
            self.initial_focus = self

        self.protocol("WM_DELETE_WINDOW", self.cancel)

        if self.parent is not None:
            self.geometry("+%d+%d" % (parent.winfo_rootx()+50,
                                      parent.winfo_rooty()+50))

        self.initial_focus.focus_set()

        self.wait_window(self)

    def destroy(self):
        '''Destroy the window'''
        self.initial_focus = None
        Toplevel.destroy(self)

    #
    # construction hooks

    def body(self, master):
        '''create dialog body.

        return widget that should have initial focus.
        This method should be overridden, and is called
        by the __init__ method.
        '''
        pass

    def buttonbox(self):
        '''add standard button box.

        override if you do not want the standard buttons
        '''

        box = Frame(self)

        w = Button(box, text="OK", width=10, command=self.ok, default=ACTIVE)
        w.pack(side=LEFT, padx=5, pady=5)
        w = Button(box, text="Cancel", width=10, command=self.cancel)
        w.pack(side=LEFT, padx=5, pady=5)

        self.bind("<Return>", self.ok)
        self.bind("<Escape>", self.cancel)

        box.pack()

    #
    # standard button semantics

    def ok(self, event=None):

        if not self.validate():
            self.initial_focus.focus_set() # put focus back
            return

        self.withdraw()
        self.update_idletasks()

        self.apply()

        self.cancel()

    def cancel(self, event=None):

        # put focus back to the parent window
        if self.parent is not None:
            self.parent.focus_set()
        self.destroy()

    #
    # command hooks

    def validate(self):
        '''validate the data

        This method is called automatically to validate the data before the
        dialog is destroyed. By default, it always validates OK.
        '''

        return 1 # override

    def apply(self):
        '''process the data

        This method is called automatically to process the data, *after*
        the dialog is destroyed. By default, it does nothing.
        '''

        pass # override
