#!/usr/bin/env python

import optimizers

if __name__ == "__main__":
    file = "../xml/FunctionModels.xml"
    funcFactory = optimizers.FunctionFactory()
    fromClone = 1
    funcFactory.addFunc("PowerLaw", optimizers.PowerLaw(), fromClone)
    funcFactory.addFunc("Gaussian", optimizers.Gaussian(), fromClone)
    funcFactory.addFunc("AbsEdge", optimizers.AbsEdge(), fromClone)
    funcFactory.listFunctions()
    funcFactory.readXml(file)
    funcFactory.listFunctions()
    gaussObj = funcFactory.create("Generic Gaussian")
    dArg = optimizers.dArg
    gauss = lambda x:gaussObj(dArg(x))
    import numarray
    x = numarray.arange(100)/99.*1. + 6.
    y = numarray.array( map(lambda x:gauss(x), x) )
    for i in range(0, len(x), 10):
        print x[i], y[i]
    params = optimizers.ParameterVector()
    gaussObj.getParams(params)
        
#    import hippoplot
#    plotter = hippoplot.hippoplot()
#    plotter.scatter(x, y, "x", "y")

