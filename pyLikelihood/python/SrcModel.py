"""
SourceModel interface to allow for manipulation of fit parameters.

@author J. Chiang <jchiang@slac.stanford.edu>
"""
#
# $Header$
#
import sys
import pyLikelihood as pyLike

def ids(istart=0):
    i = istart - 1
    while True:
        i += 1
        yield(i)

class SourceModel(object):
    def __init__(self, logLike):
        self.logLike = logLike
        self._loadSources()
    def delete(self, source):
        src = self.logLike.deleteSource(source)
        self._loadSources()
        return src
    def add(self, source):
        try:
            print "adding ", source.getName()
            self.logLike.addSource(source)
            self._loadSources()
        except:
            pass
    def _loadSources(self):
        srcNames = pyLike.StringVector()
        self.logLike.getSrcNames(srcNames)
        self.srcNames = tuple(srcNames)
        self.srcs = {}
        for name in srcNames:
            self.srcs[name] = Source(self.logLike.getSource(name))
        self._walk()
        self.printFreeOnly = False
    def _walk(self):
        indx = ids()
        self.params = []
        for srcName in self.srcNames:
            src = self[srcName]
            for funcName in src.funcs:
                if funcName == "Spectrum":
                    func = src.funcs[funcName]
                    for param in func.paramNames:
                        self.params.append(func.getParam(param))
                        src.funcs[funcName].appendParId(indx.next())
    def __setitem__(self, indx, value):
        self.params[indx].setValue(value)
        self.params[indx].setError(0)
    def __getitem__(self, srcName):
        try:
            return self.params[srcName]
        except:
            try:
                return self.srcs[self._findSrc(srcName)]
            except:
                pass
    def _findSrc(self, name):
        for item in self.srcNames:
            if item.find(name) != -1:
                return item
    def __repr__(self):
        lines = []
        for src in self.srcNames:
            lines.append(self[src].__repr__('   ', self.printFreeOnly))
        return "\n".join(lines)

class Source(object):
    def __init__(self, src):
        self.src = src
        funcs = src.getSrcFuncs()
        self.funcs = {}
        for item in funcs.keys():
            self.funcs[item] = Function(funcs[item], src.getName())
    def __getitem__(self, name):
        return self.funcs[name]
    def __repr__(self, prefix='   ', free_only=False):
        lines = [self.src.getName()]
        for item in self.funcs:
            if item == "Spectrum":
                lines.append(prefix + item + ": " + self[item].genericName())
                lines.append(self[item].__repr__(prefix, free_only))
        return "\n".join(lines) + "\n"
    def __getattr__(self, attrname):
        return getattr(self.src, attrname)

class Function(object):
    def __init__(self, func, srcName=None):
        self.func = func
        self.srcName = srcName
        names = pyLike.StringVector()
        func.getParamNames(names)
        self.paramNames = list(names)
        self.params = {}
        for name in self.paramNames:
            self.params[name] = Parameter(self.func.getParam(name), srcName)
        self._parIds = []
    def __getitem__(self, name):
        return self.func.getParamValue(name)
    def __setitem__(self, name, value):
        self.func.setParam(name, value)
    def getParam(self, name):
        return self.params[name]
    def appendParId(self, indx):
        self._parIds.append(indx)
    def __repr__(self, prefix='', free_only=False):
        lines = []
        for indx, parName in zip(self._parIds, self.paramNames):
            par = self.getParam(parName)
            if not free_only or par.isFree():
                lines.append("%-3i%s%s" % (indx, prefix, par.__repr__()))
        return "\n".join(lines)
    def __getattr__(self, attrname):
        return getattr(self.func, attrname)

class Parameter(object):
    def __init__(self, parameter, srcName=None):
        self.parameter = parameter
        self.srcName = srcName
    def __repr__(self):
        par = self.parameter
        desc = ("%10s: %10.3e " % (par.getName(), par.getValue()) +
                "%10.3e " % par.error() +
                "%10.3e %10.3e " % par.getBounds() +
                "(%10.3e)" % par.getScale())
        if not par.isFree():
            desc += " fixed"
        return desc
    def setFree(self, value):
        self.parameter.setFree(value)
        if not value:
            self.parameter.setError(0)
    def value(self):
        return self.parameter.getValue()
    def __getattr__(self, attrname):
        return getattr(self.parameter, attrname)
