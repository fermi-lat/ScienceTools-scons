// Mainpage for doxygen

/** @mainpage package optimizers

 @authors James Chiang, Pat Nolan

 @section intro Introduction

 This package provides optimization methods for minimizing objective
 functions.

 @section classes Important Classes

   - optimizers::Function Objects of this class act as "function
   objects" in that the function call operator () is overloaded so
   that instances behave like ordinary C functions.  Several methods
   are also provided for accessing the model Parameters and
   derivatives with respect to those Parameters, either singly or in
   groups.  The behavior of this class is greatly facilitated by the
   Parameter and Arg classes.

   - optimizers::Parameter This is essentially an n-tuple containing
   model parameter information (and access methods) comprising the
   parameter value, scale factor, name, upper and lower bounds and
   whether the parameter is to be considered free or fixed in the
   fitting process.

   - optimizers::Arg This class wraps arguments to Function objects
   so that Function's derivative passing mechanisms can be inherited
   by subclasses regardless of the actual type of the underlying
   argument.

   - optimizers::Statistic Subclasses of this are the objective
   functions to be optimized in order to estimate model parameters.
   Although these are in the Function hierarchy, their behavior
   differs in that _unwrapped_ (i.e., not Arg) vectors of the
   Parameters themselves are passed via the function call operator,
   ().

   - optimizers::Optimizer An abstract base class for the algorithms
   which maximize the objective functions.  After an Optimizer object
   is constructed using a Statistic object, its find_min() method will
   adjust the values of the optimizers::Parameter s.  It may not
   converge to a satisfactory maximum, so it is important to check for
   exceptional conditions.

 <hr>
 @section notes Release Notes
 @verbinclude release.notes

 <hr>
 @section requirements requirements
 @verbinclude requirements

 <hr> 
 */
