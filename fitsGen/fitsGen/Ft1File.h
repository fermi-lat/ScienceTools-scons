/**
 * @file Ft1File.h
 * @brief Declaration of FT1 file abstraction
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef fitsGen_Ft1File_h
#define fitsGen_Ft1File_h

#include "fitsGen/FtFileBase.h"

namespace fitsGen {

/**
 * @class Ft1File
 * @brief Abstraction/interface layer for using tip to write FT1
 * files.
 *
 * @author J. Chiang
 */

class Ft1File : public FtFileBase {

public:

   Ft1File(const std::string & outfile, long nrows=0);

   virtual void close();

private:

   void verifyObsTimes();

};

} // namespace fitsGen

#endif // fitsGen_Ft1File_h
