// Data types which are used by f2c.  It's dangerous to include
// f2c.h in C++ code, so we just extract the parts we need.

#ifndef F2C_TYPES_H
#define F2C_TYPES_H
    typedef short ftnlen;        // length of a Fortran string
    typedef long int logical;   // Fortran logical type
    typedef long int integer;  
#endif // F2C_TYPES_H
