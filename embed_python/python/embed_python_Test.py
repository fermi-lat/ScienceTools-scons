print "this is embed_python_Test.py"
# see if this works
import math

# make a list of strings
names = ['one', 'two', 'three']

# a list of values
values = range(5)

def callable():
    print "callable function was called!"

#define a few attributes
x=99.
z = 'test string'

# make a class with attributes
class A(object):
    a = 99
    b = -99
    c ='a name'
    
# test creating a dictionary

mymap = {'a':1, 'b':2.0}
    
print 'directory ', dir()
print
# check that the argv array was passed
import sys
print 'sys.argv %s' % sys.argv
    
