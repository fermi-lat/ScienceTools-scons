"""
Wrapper package for ds9 XPA commands.  The XPA interface is provided
by Russell Owen's RO.DS9 package.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
import os
from RO.DS9 import *

def display(file, extension=None):
    if extension is not None:
        xpaset('file %s[%s]' % (file, extension))
    else:
        xpaset("file %s" % file)
        
def zoom(maglevel=1):
    try:
        if maglevel >= 1:
            xpaset("zoom to %i" % maglevel)
    except:
        pass

def new_frame():
    xpaset("frame new")

frame_cmds = ("tile", "single", "blink")

for cmd in frame_cmds:
    foo = 'def %s(): xpaset("%s")' % (cmd, cmd)
    exec(foo)

def delete_frame(frame=None):
    if frame is None:
        xpaset("frame delete")
    else:
        xpaset("frame delete " + `frame`)
    
def cube_level(level):
    xpaset("datacube " + `level`)

def load_regions(file):
    xpaset("regions load " + file)

def save_regions(file='ds9.reg'):
    xpaset("wcs sky fk5")
    xpaset("regions save " + file)

def clear_regions():
    xpaset("regions delete all")

def cd(dir=None):
    if dir is None:
        return xpaget("cd").strip()
    else:
        xpaset("cd " + dir)

if __name__ == "__main__":
    filename = "sourceMaps.fits"
    display(filename)
    cube_level(4)
    zoom(2)
    load_regions("ds9.reg")
    new_frame()
    display(filename, 3)
    tile()
