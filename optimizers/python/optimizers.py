# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _optimizers
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class Arg(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Arg, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Arg, name)
    def __init__(self,*args):
        _swig_setattr(self, Arg, 'this', apply(_optimizers.new_Arg,args))
        _swig_setattr(self, Arg, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Arg):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Arg instance at %s>" % (self.this,)

class ArgPtr(Arg):
    def __init__(self,this):
        _swig_setattr(self, Arg, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Arg, 'thisown', 0)
        _swig_setattr(self, Arg,self.__class__,Arg)
_optimizers.Arg_swigregister(ArgPtr)

class dArg(Arg):
    __swig_setmethods__ = {}
    for _s in [Arg]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, dArg, name, value)
    __swig_getmethods__ = {}
    for _s in [Arg]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, dArg, name)
    def __init__(self,*args):
        _swig_setattr(self, dArg, 'this', apply(_optimizers.new_dArg,args))
        _swig_setattr(self, dArg, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_dArg):
        try:
            if self.thisown: destroy(self)
        except: pass
    def getValue(*args): return apply(_optimizers.dArg_getValue,args)
    def __repr__(self):
        return "<C dArg instance at %s>" % (self.this,)

class dArgPtr(dArg):
    def __init__(self,this):
        _swig_setattr(self, dArg, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, dArg, 'thisown', 0)
        _swig_setattr(self, dArg,self.__class__,dArg)
_optimizers.dArg_swigregister(dArgPtr)

class Exception:
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Exception, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Exception, name)
    def __init__(self,*args):
        _swig_setattr(self, Exception, 'this', apply(_optimizers.new_Exception,args))
        _swig_setattr(self, Exception, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Exception):
        try:
            if self.thisown: destroy(self)
        except: pass
    def what(*args): return apply(_optimizers.Exception_what,args)
    def code(*args): return apply(_optimizers.Exception_code,args)
    def __repr__(self):
        return "<C Exception instance at %s>" % (self.this,)

class ExceptionPtr(Exception):
    def __init__(self,this):
        _swig_setattr(self, Exception, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Exception, 'thisown', 0)
        _swig_setattr(self, Exception,self.__class__,Exception)
_optimizers.Exception_swigregister(ExceptionPtr)

class OutOfBounds(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, OutOfBounds, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, OutOfBounds, name)
    def __init__(self,*args):
        _swig_setattr(self, OutOfBounds, 'this', apply(_optimizers.new_OutOfBounds,args))
        _swig_setattr(self, OutOfBounds, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_OutOfBounds):
        try:
            if self.thisown: destroy(self)
        except: pass
    def value(*args): return apply(_optimizers.OutOfBounds_value,args)
    def minValue(*args): return apply(_optimizers.OutOfBounds_minValue,args)
    def maxValue(*args): return apply(_optimizers.OutOfBounds_maxValue,args)
    VALUE_ERROR = _optimizers.OutOfBounds_VALUE_ERROR
    BOUNDS_ERROR = _optimizers.OutOfBounds_BOUNDS_ERROR
    def __repr__(self):
        return "<C OutOfBounds instance at %s>" % (self.this,)

class OutOfBoundsPtr(OutOfBounds):
    def __init__(self,this):
        _swig_setattr(self, OutOfBounds, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, OutOfBounds, 'thisown', 0)
        _swig_setattr(self, OutOfBounds,self.__class__,OutOfBounds)
_optimizers.OutOfBounds_swigregister(OutOfBoundsPtr)

class ParameterNotFound(Exception):
    __swig_setmethods__ = {}
    for _s in [Exception]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ParameterNotFound, name, value)
    __swig_getmethods__ = {}
    for _s in [Exception]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ParameterNotFound, name)
    def __init__(self,*args):
        _swig_setattr(self, ParameterNotFound, 'this', apply(_optimizers.new_ParameterNotFound,args))
        _swig_setattr(self, ParameterNotFound, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_ParameterNotFound):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ParameterNotFound instance at %s>" % (self.this,)

class ParameterNotFoundPtr(ParameterNotFound):
    def __init__(self,this):
        _swig_setattr(self, ParameterNotFound, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ParameterNotFound, 'thisown', 0)
        _swig_setattr(self, ParameterNotFound,self.__class__,ParameterNotFound)
_optimizers.ParameterNotFound_swigregister(ParameterNotFoundPtr)

class Parameter(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Parameter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Parameter, name)
    def __init__(self,*args):
        _swig_setattr(self, Parameter, 'this', apply(_optimizers.new_Parameter,args))
        _swig_setattr(self, Parameter, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Parameter):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setName(*args): return apply(_optimizers.Parameter_setName,args)
    def getName(*args): return apply(_optimizers.Parameter_getName,args)
    def setValue(*args): return apply(_optimizers.Parameter_setValue,args)
    def getValue(*args): return apply(_optimizers.Parameter_getValue,args)
    def setScale(*args): return apply(_optimizers.Parameter_setScale,args)
    def getScale(*args): return apply(_optimizers.Parameter_getScale,args)
    def setTrueValue(*args): return apply(_optimizers.Parameter_setTrueValue,args)
    def getTrueValue(*args): return apply(_optimizers.Parameter_getTrueValue,args)
    def setBounds(*args): return apply(_optimizers.Parameter_setBounds,args)
    def getBounds(*args): return apply(_optimizers.Parameter_getBounds,args)
    def setFree(*args): return apply(_optimizers.Parameter_setFree,args)
    def isFree(*args): return apply(_optimizers.Parameter_isFree,args)
    def __repr__(self):
        return "<C Parameter instance at %s>" % (self.this,)

class ParameterPtr(Parameter):
    def __init__(self,this):
        _swig_setattr(self, Parameter, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Parameter, 'thisown', 0)
        _swig_setattr(self, Parameter,self.__class__,Parameter)
_optimizers.Parameter_swigregister(ParameterPtr)

class Function(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Function, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Function, name)
    def __del__(self, destroy= _optimizers.delete_Function):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setName(*args): return apply(_optimizers.Function_setName,args)
    def getName(*args): return apply(_optimizers.Function_getName,args)
    def setParam(*args): return apply(_optimizers.Function_setParam,args)
    def getParamValue(*args): return apply(_optimizers.Function_getParamValue,args)
    def setParamScale(*args): return apply(_optimizers.Function_setParamScale,args)
    def setParamTrueValue(*args): return apply(_optimizers.Function_setParamTrueValue,args)
    def getNumParams(*args): return apply(_optimizers.Function_getNumParams,args)
    def setParamValues(*args): return apply(_optimizers.Function_setParamValues,args)
    def setParamValues_(*args): return apply(_optimizers.Function_setParamValues_,args)
    def setParams(*args): return apply(_optimizers.Function_setParams,args)
    def getParamNames(*args): return apply(_optimizers.Function_getParamNames,args)
    def getParamValues(*args): return apply(_optimizers.Function_getParamValues,args)
    def getParams(*args): return apply(_optimizers.Function_getParams,args)
    def getNumFreeParams(*args): return apply(_optimizers.Function_getNumFreeParams,args)
    def setFreeParamValues(*args): return apply(_optimizers.Function_setFreeParamValues,args)
    def setFreeParamValues_(*args): return apply(_optimizers.Function_setFreeParamValues_,args)
    def getFreeParamNames(*args): return apply(_optimizers.Function_getFreeParamNames,args)
    def getFreeParamValues(*args): return apply(_optimizers.Function_getFreeParamValues,args)
    def getFreeParams(*args): return apply(_optimizers.Function_getFreeParams,args)
    def setFreeParams(*args): return apply(_optimizers.Function_setFreeParams,args)
    def value(*args): return apply(_optimizers.Function_value,args)
    def __call__(*args): return apply(_optimizers.Function___call__,args)
    def derivByParam(*args): return apply(_optimizers.Function_derivByParam,args)
    def getDerivs(*args): return apply(_optimizers.Function_getDerivs,args)
    def getFreeDerivs(*args): return apply(_optimizers.Function_getFreeDerivs,args)
    def integral(*args): return apply(_optimizers.Function_integral,args)
    def clone(*args): return apply(_optimizers.Function_clone,args)
    None = _optimizers.Function_None
    Addend = _optimizers.Function_Addend
    Factor = _optimizers.Function_Factor
    def funcType(*args): return apply(_optimizers.Function_funcType,args)
    def argType(*args): return apply(_optimizers.Function_argType,args)
    def setParamBounds(*args): return apply(_optimizers.Function_setParamBounds,args)
    def getParam(*args): return apply(_optimizers.Function_getParam,args)
    __swig_getmethods__["string"] = lambda x: _optimizers.Function_string
    if _newclass:string = staticmethod(_optimizers.Function_string)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C Function instance at %s>" % (self.this,)

class FunctionPtr(Function):
    def __init__(self,this):
        _swig_setattr(self, Function, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Function, 'thisown', 0)
        _swig_setattr(self, Function,self.__class__,Function)
_optimizers.Function_swigregister(FunctionPtr)
Function_string = _optimizers.Function_string


class CompositeFunction(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, CompositeFunction, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, CompositeFunction, name)
    def __del__(self, destroy= _optimizers.delete_CompositeFunction):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setParam(*args): return apply(_optimizers.CompositeFunction_setParam,args)
    def setParamValues_(*args): return apply(_optimizers.CompositeFunction_setParamValues_,args)
    def setFreeParamValues_(*args): return apply(_optimizers.CompositeFunction_setFreeParamValues_,args)
    def getParam(*args): return apply(_optimizers.CompositeFunction_getParam,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C CompositeFunction instance at %s>" % (self.this,)

class CompositeFunctionPtr(CompositeFunction):
    def __init__(self,this):
        _swig_setattr(self, CompositeFunction, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CompositeFunction, 'thisown', 0)
        _swig_setattr(self, CompositeFunction,self.__class__,CompositeFunction)
_optimizers.CompositeFunction_swigregister(CompositeFunctionPtr)

class ProductFunction(CompositeFunction):
    __swig_setmethods__ = {}
    for _s in [CompositeFunction]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ProductFunction, name, value)
    __swig_getmethods__ = {}
    for _s in [CompositeFunction]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ProductFunction, name)
    def __init__(self,*args):
        _swig_setattr(self, ProductFunction, 'this', apply(_optimizers.new_ProductFunction,args))
        _swig_setattr(self, ProductFunction, 'thisown', 1)
    def value(*args): return apply(_optimizers.ProductFunction_value,args)
    def clone(*args): return apply(_optimizers.ProductFunction_clone,args)
    def __del__(self, destroy= _optimizers.delete_ProductFunction):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ProductFunction instance at %s>" % (self.this,)

class ProductFunctionPtr(ProductFunction):
    def __init__(self,this):
        _swig_setattr(self, ProductFunction, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ProductFunction, 'thisown', 0)
        _swig_setattr(self, ProductFunction,self.__class__,ProductFunction)
_optimizers.ProductFunction_swigregister(ProductFunctionPtr)

class SumFunction(CompositeFunction):
    __swig_setmethods__ = {}
    for _s in [CompositeFunction]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, SumFunction, name, value)
    __swig_getmethods__ = {}
    for _s in [CompositeFunction]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, SumFunction, name)
    def __init__(self,*args):
        _swig_setattr(self, SumFunction, 'this', apply(_optimizers.new_SumFunction,args))
        _swig_setattr(self, SumFunction, 'thisown', 1)
    def value(*args): return apply(_optimizers.SumFunction_value,args)
    def integral(*args): return apply(_optimizers.SumFunction_integral,args)
    def clone(*args): return apply(_optimizers.SumFunction_clone,args)
    def __del__(self, destroy= _optimizers.delete_SumFunction):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C SumFunction instance at %s>" % (self.this,)

class SumFunctionPtr(SumFunction):
    def __init__(self,this):
        _swig_setattr(self, SumFunction, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, SumFunction, 'thisown', 0)
        _swig_setattr(self, SumFunction,self.__class__,SumFunction)
_optimizers.SumFunction_swigregister(SumFunctionPtr)

class FunctionTest(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FunctionTest, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FunctionTest, name)
    def __init__(self,*args):
        _swig_setattr(self, FunctionTest, 'this', apply(_optimizers.new_FunctionTest,args))
        _swig_setattr(self, FunctionTest, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_FunctionTest):
        try:
            if self.thisown: destroy(self)
        except: pass
    def parameters(*args): return apply(_optimizers.FunctionTest_parameters,args)
    def freeParameters(*args): return apply(_optimizers.FunctionTest_freeParameters,args)
    def funcEvaluations(*args): return apply(_optimizers.FunctionTest_funcEvaluations,args)
    def derivatives(*args): return apply(_optimizers.FunctionTest_derivatives,args)
    def __repr__(self):
        return "<C FunctionTest instance at %s>" % (self.this,)

class FunctionTestPtr(FunctionTest):
    def __init__(self,this):
        _swig_setattr(self, FunctionTest, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FunctionTest, 'thisown', 0)
        _swig_setattr(self, FunctionTest,self.__class__,FunctionTest)
_optimizers.FunctionTest_swigregister(FunctionTestPtr)

class Mcmc(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Mcmc, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Mcmc, name)
    def __init__(self,*args):
        _swig_setattr(self, Mcmc, 'this', apply(_optimizers.new_Mcmc,args))
        _swig_setattr(self, Mcmc, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Mcmc):
        try:
            if self.thisown: destroy(self)
        except: pass
    def addPriors(*args): return apply(_optimizers.Mcmc_addPriors,args)
    def generateSamples(*args): return apply(_optimizers.Mcmc_generateSamples,args)
    def setTransitionWidths(*args): return apply(_optimizers.Mcmc_setTransitionWidths,args)
    def getTransitionWidths(*args): return apply(_optimizers.Mcmc_getTransitionWidths,args)
    __swig_getmethods__["writeSamples"] = lambda x: _optimizers.Mcmc_writeSamples
    if _newclass:writeSamples = staticmethod(_optimizers.Mcmc_writeSamples)
    def __repr__(self):
        return "<C Mcmc instance at %s>" % (self.this,)

class McmcPtr(Mcmc):
    def __init__(self,this):
        _swig_setattr(self, Mcmc, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Mcmc, 'thisown', 0)
        _swig_setattr(self, Mcmc,self.__class__,Mcmc)
_optimizers.Mcmc_swigregister(McmcPtr)
Mcmc_writeSamples = _optimizers.Mcmc_writeSamples


class Optimizer(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Optimizer, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Optimizer, name)
    def __del__(self, destroy= _optimizers.delete_Optimizer):
        try:
            if self.thisown: destroy(self)
        except: pass
    def find_min(*args): return apply(_optimizers.Optimizer_find_min,args)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<C Optimizer instance at %s>" % (self.this,)

class OptimizerPtr(Optimizer):
    def __init__(self,this):
        _swig_setattr(self, Optimizer, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Optimizer, 'thisown', 0)
        _swig_setattr(self, Optimizer,self.__class__,Optimizer)
_optimizers.Optimizer_swigregister(OptimizerPtr)

class Lbfgs(Optimizer):
    __swig_setmethods__ = {}
    for _s in [Optimizer]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Lbfgs, name, value)
    __swig_getmethods__ = {}
    for _s in [Optimizer]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Lbfgs, name)
    def __init__(self,*args):
        _swig_setattr(self, Lbfgs, 'this', apply(_optimizers.new_Lbfgs,args))
        _swig_setattr(self, Lbfgs, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Lbfgs):
        try:
            if self.thisown: destroy(self)
        except: pass
    def setMaxVarMetCorr(*args): return apply(_optimizers.Lbfgs_setMaxVarMetCorr,args)
    def setPgtol(*args): return apply(_optimizers.Lbfgs_setPgtol,args)
    def setMaxIterations(*args): return apply(_optimizers.Lbfgs_setMaxIterations,args)
    def getRetCode(*args): return apply(_optimizers.Lbfgs_getRetCode,args)
    def getErrorString(*args): return apply(_optimizers.Lbfgs_getErrorString,args)
    def find_min(*args): return apply(_optimizers.Lbfgs_find_min,args)
    LBFGS_NORMAL = _optimizers.Lbfgs_LBFGS_NORMAL
    LBFGS_ABNO = _optimizers.Lbfgs_LBFGS_ABNO
    LBFGS_ERROR = _optimizers.Lbfgs_LBFGS_ERROR
    LBFGS_TOOMANY = _optimizers.Lbfgs_LBFGS_TOOMANY
    LBFGS_UNKNOWN = _optimizers.Lbfgs_LBFGS_UNKNOWN
    def __repr__(self):
        return "<C Lbfgs instance at %s>" % (self.this,)

class LbfgsPtr(Lbfgs):
    def __init__(self,this):
        _swig_setattr(self, Lbfgs, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Lbfgs, 'thisown', 0)
        _swig_setattr(self, Lbfgs,self.__class__,Lbfgs)
_optimizers.Lbfgs_swigregister(LbfgsPtr)

setulb_ = _optimizers.setulb_

class Minuit(Optimizer):
    __swig_setmethods__ = {}
    for _s in [Optimizer]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Minuit, name, value)
    __swig_getmethods__ = {}
    for _s in [Optimizer]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Minuit, name)
    def __init__(self,*args):
        _swig_setattr(self, Minuit, 'this', apply(_optimizers.new_Minuit,args))
        _swig_setattr(self, Minuit, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Minuit):
        try:
            if self.thisown: destroy(self)
        except: pass
    def find_min(*args): return apply(_optimizers.Minuit_find_min,args)
    def setMaxEval(*args): return apply(_optimizers.Minuit_setMaxEval,args)
    def getQuality(*args): return apply(_optimizers.Minuit_getQuality,args)
    def getDistance(*args): return apply(_optimizers.Minuit_getDistance,args)
    def getUncertainty(*args): return apply(_optimizers.Minuit_getUncertainty,args)
    MINUIT_NOTCALC = _optimizers.Minuit_MINUIT_NOTCALC
    MINUIT_DIAG = _optimizers.Minuit_MINUIT_DIAG
    MINUIT_FORCEDPOS = _optimizers.Minuit_MINUIT_FORCEDPOS
    MINUIT_NORMAL = _optimizers.Minuit_MINUIT_NORMAL
    def __repr__(self):
        return "<C Minuit instance at %s>" % (self.this,)

class MinuitPtr(Minuit):
    def __init__(self,this):
        _swig_setattr(self, Minuit, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Minuit, 'thisown', 0)
        _swig_setattr(self, Minuit,self.__class__,Minuit)
_optimizers.Minuit_swigregister(MinuitPtr)

fcn = _optimizers.fcn

mninit_ = _optimizers.mninit_

mnparm_ = _optimizers.mnparm_

mncomd_ = _optimizers.mncomd_

mnexcm_ = _optimizers.mnexcm_

mnpout_ = _optimizers.mnpout_

mnstat_ = _optimizers.mnstat_

mnseti_ = _optimizers.mnseti_

mnpars_ = _optimizers.mnpars_

mnemat_ = _optimizers.mnemat_

mnerrs_ = _optimizers.mnerrs_

mncont_ = _optimizers.mncont_

intrac_ = _optimizers.intrac_

class Drmngb(Optimizer):
    __swig_setmethods__ = {}
    for _s in [Optimizer]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Drmngb, name, value)
    __swig_getmethods__ = {}
    for _s in [Optimizer]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Drmngb, name)
    def __init__(self,*args):
        _swig_setattr(self, Drmngb, 'this', apply(_optimizers.new_Drmngb,args))
        _swig_setattr(self, Drmngb, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_Drmngb):
        try:
            if self.thisown: destroy(self)
        except: pass
    def find_min(*args): return apply(_optimizers.Drmngb_find_min,args)
    def getUncertainty(*args): return apply(_optimizers.Drmngb_getUncertainty,args)
    def getRetCode(*args): return apply(_optimizers.Drmngb_getRetCode,args)
    XCONV = _optimizers.Drmngb_XCONV
    RELCONV = _optimizers.Drmngb_RELCONV
    BOTHCONV = _optimizers.Drmngb_BOTHCONV
    ABSCONV = _optimizers.Drmngb_ABSCONV
    SINGCONV = _optimizers.Drmngb_SINGCONV
    FALSECONV = _optimizers.Drmngb_FALSECONV
    EVALLIM = _optimizers.Drmngb_EVALLIM
    ITLIM = _optimizers.Drmngb_ITLIM
    STOPX = _optimizers.Drmngb_STOPX
    ALLOCATED = _optimizers.Drmngb_ALLOCATED
    LIVSMALL = _optimizers.Drmngb_LIVSMALL
    LVSMALL = _optimizers.Drmngb_LVSMALL
    RESTARTATT = _optimizers.Drmngb_RESTARTATT
    DNEG = _optimizers.Drmngb_DNEG
    VORANGE = _optimizers.Drmngb_VORANGE
    CANTCOMPUTEF = _optimizers.Drmngb_CANTCOMPUTEF
    BADPAR = _optimizers.Drmngb_BADPAR
    CANTCOMPUTEG = _optimizers.Drmngb_CANTCOMPUTEG
    BADARR = _optimizers.Drmngb_BADARR
    BADPAR1 = _optimizers.Drmngb_BADPAR1
    BUGS = _optimizers.Drmngb_BUGS
    def __repr__(self):
        return "<C Drmngb instance at %s>" % (self.this,)

class DrmngbPtr(Drmngb):
    def __init__(self,this):
        _swig_setattr(self, Drmngb, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Drmngb, 'thisown', 0)
        _swig_setattr(self, Drmngb,self.__class__,Drmngb)
_optimizers.Drmngb_swigregister(DrmngbPtr)

drmngb_ = _optimizers.drmngb_

divset_ = _optimizers.divset_

dpptri_ = _optimizers.dpptri_

class AbsEdge(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, AbsEdge, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, AbsEdge, name)
    def __init__(self,*args):
        _swig_setattr(self, AbsEdge, 'this', apply(_optimizers.new_AbsEdge,args))
        _swig_setattr(self, AbsEdge, 'thisown', 1)
    def value(*args): return apply(_optimizers.AbsEdge_value,args)
    def derivByParam(*args): return apply(_optimizers.AbsEdge_derivByParam,args)
    def clone(*args): return apply(_optimizers.AbsEdge_clone,args)
    def __del__(self, destroy= _optimizers.delete_AbsEdge):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C AbsEdge instance at %s>" % (self.this,)

class AbsEdgePtr(AbsEdge):
    def __init__(self,this):
        _swig_setattr(self, AbsEdge, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AbsEdge, 'thisown', 0)
        _swig_setattr(self, AbsEdge,self.__class__,AbsEdge)
_optimizers.AbsEdge_swigregister(AbsEdgePtr)

class Gaussian(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Gaussian, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Gaussian, name)
    def __init__(self,*args):
        _swig_setattr(self, Gaussian, 'this', apply(_optimizers.new_Gaussian,args))
        _swig_setattr(self, Gaussian, 'thisown', 1)
    def value(*args): return apply(_optimizers.Gaussian_value,args)
    def derivByParam(*args): return apply(_optimizers.Gaussian_derivByParam,args)
    def integral(*args): return apply(_optimizers.Gaussian_integral,args)
    def clone(*args): return apply(_optimizers.Gaussian_clone,args)
    def __del__(self, destroy= _optimizers.delete_Gaussian):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Gaussian instance at %s>" % (self.this,)

class GaussianPtr(Gaussian):
    def __init__(self,this):
        _swig_setattr(self, Gaussian, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Gaussian, 'thisown', 0)
        _swig_setattr(self, Gaussian,self.__class__,Gaussian)
_optimizers.Gaussian_swigregister(GaussianPtr)

class MyFun(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, MyFun, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, MyFun, name)
    def __init__(self,*args):
        _swig_setattr(self, MyFun, 'this', apply(_optimizers.new_MyFun,args))
        _swig_setattr(self, MyFun, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_MyFun):
        try:
            if self.thisown: destroy(self)
        except: pass
    def value(*args): return apply(_optimizers.MyFun_value,args)
    def derivByParam(*args): return apply(_optimizers.MyFun_derivByParam,args)
    def __repr__(self):
        return "<C MyFun instance at %s>" % (self.this,)

class MyFunPtr(MyFun):
    def __init__(self,this):
        _swig_setattr(self, MyFun, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MyFun, 'thisown', 0)
        _swig_setattr(self, MyFun,self.__class__,MyFun)
_optimizers.MyFun_swigregister(MyFunPtr)

class PowerLaw(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, PowerLaw, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, PowerLaw, name)
    def __init__(self,*args):
        _swig_setattr(self, PowerLaw, 'this', apply(_optimizers.new_PowerLaw,args))
        _swig_setattr(self, PowerLaw, 'thisown', 1)
    def value(*args): return apply(_optimizers.PowerLaw_value,args)
    def derivByParam(*args): return apply(_optimizers.PowerLaw_derivByParam,args)
    def integral(*args): return apply(_optimizers.PowerLaw_integral,args)
    def clone(*args): return apply(_optimizers.PowerLaw_clone,args)
    def __del__(self, destroy= _optimizers.delete_PowerLaw):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C PowerLaw instance at %s>" % (self.this,)

class PowerLawPtr(PowerLaw):
    def __init__(self,this):
        _swig_setattr(self, PowerLaw, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PowerLaw, 'thisown', 0)
        _swig_setattr(self, PowerLaw,self.__class__,PowerLaw)
_optimizers.PowerLaw_swigregister(PowerLawPtr)

class Rosen(Function):
    __swig_setmethods__ = {}
    for _s in [Function]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Rosen, name, value)
    __swig_getmethods__ = {}
    for _s in [Function]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Rosen, name)
    def __init__(self,*args):
        _swig_setattr(self, Rosen, 'this', apply(_optimizers.new_Rosen,args))
        _swig_setattr(self, Rosen, 'thisown', 1)
    def value(*args): return apply(_optimizers.Rosen_value,args)
    def derivByParam(*args): return apply(_optimizers.Rosen_derivByParam,args)
    def __del__(self, destroy= _optimizers.delete_Rosen):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C Rosen instance at %s>" % (self.this,)

class RosenPtr(Rosen):
    def __init__(self,this):
        _swig_setattr(self, Rosen, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Rosen, 'thisown', 0)
        _swig_setattr(self, Rosen,self.__class__,Rosen)
_optimizers.Rosen_swigregister(RosenPtr)

class FunctionFactory(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FunctionFactory, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FunctionFactory, name)
    def __init__(self,*args):
        _swig_setattr(self, FunctionFactory, 'this', apply(_optimizers.new_FunctionFactory,args))
        _swig_setattr(self, FunctionFactory, 'thisown', 1)
    def __del__(self, destroy= _optimizers.delete_FunctionFactory):
        try:
            if self.thisown: destroy(self)
        except: pass
    def listFunctions(*args): return apply(_optimizers.FunctionFactory_listFunctions,args)
    def readXml(*args): return apply(_optimizers.FunctionFactory_readXml,args)
    def create(*args): return apply(_optimizers.FunctionFactory_create,args)
    def addFunc(*args): return apply(_optimizers.FunctionFactory_addFunc,args)
    def __repr__(self):
        return "<C FunctionFactory instance at %s>" % (self.this,)

class FunctionFactoryPtr(FunctionFactory):
    def __init__(self,this):
        _swig_setattr(self, FunctionFactory, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FunctionFactory, 'thisown', 0)
        _swig_setattr(self, FunctionFactory,self.__class__,FunctionFactory)
_optimizers.FunctionFactory_swigregister(FunctionFactoryPtr)

class DoubleVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DoubleVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DoubleVector, name)
    def __init__(self,*args):
        _swig_setattr(self, DoubleVector, 'this', apply(_optimizers.new_DoubleVector,args))
        _swig_setattr(self, DoubleVector, 'thisown', 1)
    def __len__(*args): return apply(_optimizers.DoubleVector___len__,args)
    def __nonzero__(*args): return apply(_optimizers.DoubleVector___nonzero__,args)
    def clear(*args): return apply(_optimizers.DoubleVector_clear,args)
    def append(*args): return apply(_optimizers.DoubleVector_append,args)
    def pop(*args): return apply(_optimizers.DoubleVector_pop,args)
    def __getitem__(*args): return apply(_optimizers.DoubleVector___getitem__,args)
    def __getslice__(*args): return apply(_optimizers.DoubleVector___getslice__,args)
    def __setitem__(*args): return apply(_optimizers.DoubleVector___setitem__,args)
    def __setslice__(*args): return apply(_optimizers.DoubleVector___setslice__,args)
    def __delitem__(*args): return apply(_optimizers.DoubleVector___delitem__,args)
    def __delslice__(*args): return apply(_optimizers.DoubleVector___delslice__,args)
    def __del__(self, destroy= _optimizers.delete_DoubleVector):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C DoubleVector instance at %s>" % (self.this,)

class DoubleVectorPtr(DoubleVector):
    def __init__(self,this):
        _swig_setattr(self, DoubleVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, DoubleVector, 'thisown', 0)
        _swig_setattr(self, DoubleVector,self.__class__,DoubleVector)
_optimizers.DoubleVector_swigregister(DoubleVectorPtr)

class DoubleVectorVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, DoubleVectorVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, DoubleVectorVector, name)
    def __init__(self,*args):
        _swig_setattr(self, DoubleVectorVector, 'this', apply(_optimizers.new_DoubleVectorVector,args))
        _swig_setattr(self, DoubleVectorVector, 'thisown', 1)
    def __len__(*args): return apply(_optimizers.DoubleVectorVector___len__,args)
    def clear(*args): return apply(_optimizers.DoubleVectorVector_clear,args)
    def append(*args): return apply(_optimizers.DoubleVectorVector_append,args)
    def __nonzero__(*args): return apply(_optimizers.DoubleVectorVector___nonzero__,args)
    def pop(*args): return apply(_optimizers.DoubleVectorVector_pop,args)
    def __getitem__(*args): return apply(_optimizers.DoubleVectorVector___getitem__,args)
    def __getslice__(*args): return apply(_optimizers.DoubleVectorVector___getslice__,args)
    def __setitem__(*args): return apply(_optimizers.DoubleVectorVector___setitem__,args)
    def __setslice__(*args): return apply(_optimizers.DoubleVectorVector___setslice__,args)
    def __delitem__(*args): return apply(_optimizers.DoubleVectorVector___delitem__,args)
    def __delslice__(*args): return apply(_optimizers.DoubleVectorVector___delslice__,args)
    def __del__(self, destroy= _optimizers.delete_DoubleVectorVector):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C DoubleVectorVector instance at %s>" % (self.this,)

class DoubleVectorVectorPtr(DoubleVectorVector):
    def __init__(self,this):
        _swig_setattr(self, DoubleVectorVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, DoubleVectorVector, 'thisown', 0)
        _swig_setattr(self, DoubleVectorVector,self.__class__,DoubleVectorVector)
_optimizers.DoubleVectorVector_swigregister(DoubleVectorVectorPtr)

class StringVector(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, StringVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, StringVector, name)
    def __init__(self,*args):
        _swig_setattr(self, StringVector, 'this', apply(_optimizers.new_StringVector,args))
        _swig_setattr(self, StringVector, 'thisown', 1)
    def __len__(*args): return apply(_optimizers.StringVector___len__,args)
    def __nonzero__(*args): return apply(_optimizers.StringVector___nonzero__,args)
    def clear(*args): return apply(_optimizers.StringVector_clear,args)
    def append(*args): return apply(_optimizers.StringVector_append,args)
    def pop(*args): return apply(_optimizers.StringVector_pop,args)
    def __getitem__(*args): return apply(_optimizers.StringVector___getitem__,args)
    def __getslice__(*args): return apply(_optimizers.StringVector___getslice__,args)
    def __setitem__(*args): return apply(_optimizers.StringVector___setitem__,args)
    def __setslice__(*args): return apply(_optimizers.StringVector___setslice__,args)
    def __delitem__(*args): return apply(_optimizers.StringVector___delitem__,args)
    def __delslice__(*args): return apply(_optimizers.StringVector___delslice__,args)
    def __del__(self, destroy= _optimizers.delete_StringVector):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C StringVector instance at %s>" % (self.this,)

class StringVectorPtr(StringVector):
    def __init__(self,this):
        _swig_setattr(self, StringVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, StringVector, 'thisown', 0)
        _swig_setattr(self, StringVector,self.__class__,StringVector)
_optimizers.StringVector_swigregister(StringVectorPtr)


