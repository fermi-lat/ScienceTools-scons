#!/usr/bin/env python

import optimizers

class PrototypeFactory:
    def __init__(self, xmlFile):
        self.funcFactory = optimizers.FunctionFactory()
        #
        # Supply the generic Functions to the factory.
        #
        self.funcFactory.addFunc("PowerLaw", optimizers.PowerLaw(), 1)
        self.funcFactory.addFunc("Gaussian", optimizers.Gaussian(), 1)
        self.funcFactory.addFunc("AbsEdge", optimizers.AbsEdge(), 1)
        #
        # Parse the xml file data.
        #
        from xml.dom import minidom
        library = minidom.parse(xmlFile)
        funcs = library.getElementsByTagName("function")
        for func in funcs:
            funcObj = self.funcFactory.create( str(func.getAttribute("type")) )
            funcObj.setName( str(func.getAttribute("name")) )
            params = func.getElementsByTagName("parameter")
            for param in params:
                paramName = str(param.getAttribute("name"))
                paramValue = float( param.getAttribute("value") )
                paramMin = float( param.getAttribute("min") )
                paramMax = float( param.getAttribute("max") )
                if (param.getAttribute("free") == "true"):
                    isFree = 1
                else:
                    isFree = 0
                paramScale = float( param.getAttribute("scale") )
                paramObj = optimizers.Parameter(paramName,
                                                paramValue, paramMin,
                                                paramMax, isFree)
                paramObj.setScale(paramScale)
                funcObj.setParam(paramObj)
            self.funcFactory.addFunc(funcObj.getName(), funcObj)

    def listFunctions(self):
        funcNames = optimizers.StringVector()
        self.funcFactory.getFunctionNames(funcNames)
        for name in funcNames:
            print name

    def create(self, name):
        return self.funcFactory.create(name)

if __name__ == "__main__":
    #
    # Read in the customized prototypes.
    #
    file = "../xml/FunctionModels.xml"
    prototypes = PrototypeFactory(file)
    prototypes.listFunctions()
    #
    # Exercise one of the custom guys.
    #
    gaussObj = prototypes.create("Generic Gaussian")
    dArg = optimizers.dArg
    gauss = lambda x:gaussObj(dArg(x))
    import numarray
    x = numarray.arange(100)/99.*1. + 6.
    y = numarray.array( map(lambda x:gauss(x), x) )
    for i in range(0, len(x), 10):
        print x[i], y[i]
#    import hippoplot
#    plotter = hippoplot.hippoplot()
#    plotter.scatter(x, y, "x", "y")
