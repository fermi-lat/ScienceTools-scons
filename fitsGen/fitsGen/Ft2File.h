/**
 * @file Ft2File.h
 * @brief Declaration of FT1 file abstraction
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_Ft2File_h
#define fitsGen_Ft2File_h

#include "fitsGen/FtFileBase.h"

namespace fitsGen {

/**
 * @class Ft2File
 * @brief Abstraction/interface layer for using tip to write FT2
 * files.
 *
 * @author J. Chiang
 */

class Ft2File : public FtFileBase {

public:

   Ft2File(const std::string & outfile,
           long nrows=0, 
           const std::string & extname="SC_DATA",
           const std::string & templateFile="ft2.tpl")
      : FtFileBase(outfile, nrows) {
      init(templateFile, extname);
   }

};

} // namespace fitsGen

#endif // fitsGen_Ft2File_h
