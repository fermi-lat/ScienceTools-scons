from Tkinter import *

class STToplevel(Toplevel):
#     def __init__(self):
#         Toplevel.__init__()
        
    def stop(self):
        self.quit()
#        self.destroy()
        list = self.winfo_children()
        for item in list:
            item.destroy()
