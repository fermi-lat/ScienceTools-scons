/** @file Constants.h
    @brief some enums needed by Fits wrapper code
*/

#ifndef TABLE_CONSTANTS_H
#define TABLE_CONSTANTS_H 

namespace table
{

	typedef enum {Read, Write, Reads, NEW, CREATE, RECREATE, UPDATE, READ} RWmode;

	typedef enum {Success, Failure} StatusCode;
	// ValueType definition taken From CCfits - Author Ben Dorman
	typedef enum {Tnull, Tbit, Tbyte, Tlogical, Tstring, Tushort, Tdouble, Tuint, Tint, 
		Tulong, Tlong, Tfloat, Tshort, Tcomplex, Tdblcomplex, VTbit, VTbyte, VTlogical, 
		VTushort, VTshort, VTuint, VTint, VTulong, VTlong, VTfloat, VTdouble, VTcomplex, 
		VTdblcomplex} ValueType;

	typedef enum {Ibyte, Ishort, Ilong, Ifloat, Idouble, Iushort, Iulong} ImageType;

}
#endif
