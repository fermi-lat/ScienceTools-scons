/** \file TestColumn.cxx
    \brief Implementation of class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#include <iostream>
#include <sstream>

#include "fitsio.h"

#include "FitsColumn.h"
#include "TestColumn.h"

namespace tip {

  TestColumn::~TestColumn() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestColumn::test(int status) {
    setStatus(status);

    copyDataFile(getDataDir() + "a1.pha", "a1-copy.pha");
    copyDataFile(getDataDir() + "aeff_DC1.fits", "aeff_DC1-copy.fits");

    return getStatus();
  }

  void TestColumn::copyDataFile(const std::string & in_file, const std::string & out_file) {
    int status = 0;
    std::string copy_file = std::string("!") + out_file + "(" + in_file + ")";

    // Create output file using input file as a template.
    fitsfile * out_fp = 0;
    fits_create_file(&out_fp, const_cast<char *>(copy_file.c_str()), &status);
    if (0 != status) {
      setStatus(status);
      throw std::runtime_error(std::string("Unexpected: TestColumn::copyDataFile could not create file ") + copy_file);
    }

    // Open the input file.
    fitsfile * in_fp = 0;
    fits_open_file(&in_fp, const_cast<char *>(in_file.c_str()), READONLY, &status);
    if (0 != status) {
      setStatus(status);
      fits_close_file(out_fp, &status);
      throw std::runtime_error(std::string("Unexpected: TestColumn::copyDataFile could not open file ") + copy_file);
    }

    for(bool done = false; !done;) {
      // Move to next extension in input file.
      fits_movrel_hdu(in_fp, 1, 0, &status);
      if (0 != status) {
        if (END_OF_FILE == status) {
          done = true;
          continue;
        }
        setStatus(status);
        fits_close_file(in_fp, &status);
        fits_close_file(out_fp, &status);
        throw std::runtime_error(std::string("Unexpected: TestColumn::copyDataFile could not move to next HDU of ") + in_file);
      }
  
      // Move to next extension in output file.
      fits_movrel_hdu(out_fp, 1, 0, &status);
      if (0 != status) {
        setStatus(status);
        fits_close_file(in_fp, &status);
        fits_close_file(out_fp, &status);
        throw std::runtime_error(std::string("Unexpected: TestColumn::copyDataFile could not move to next HDU of ") + out_file);
      }
  
      // Get number of columns to iterate over.
      int num_cols = 0;
      fits_get_num_cols(in_fp, &num_cols, &status);
      if (0 != status) {
        setStatus(status);
        fits_close_file(in_fp, &status);
        fits_close_file(out_fp, &status);
        throw std::runtime_error("Unexpected: TestColumn::copyDataFile could not get number of columns in test file.");
      }
  
      // Get number of rows to iterate over.
      long num_rows = 0;
      fits_get_num_rows(in_fp, &num_rows, &status);
      if (0 != status) {
        setStatus(status);
        fits_close_file(in_fp, &status);
        fits_close_file(out_fp, &status);
        throw std::runtime_error("Unexpected: TestColumn::copyDataFile could not get number of rows in test file.");
      }
  
      // Create column objects for each column in input and output files.
      for (int ii = 0; ii < num_cols; ++ii) {
        int type_code = 0;
        long repeat = 0;
        // Get info about the type. This will be used to create the column object.
        fits_get_coltype(in_fp, ii + 1, &type_code, &repeat, 0, &status);
        if (0 != status) {
          setStatus(status);
          fits_close_file(in_fp, &status);
          fits_close_file(out_fp, &status);
          std::ostringstream os;
          os << "Unexpected: TestColumn::copyDataFile could not get type of column " << ii << " in test file.";
          throw std::runtime_error(os.str());
        }
      
        // Handle special case of variable length columns.
        if (type_code < 0) {
          type_code *= -1;
          repeat = 0;
        }
  
        const IColumn * in_col = 0;
        IColumn * out_col = 0;

        switch (type_code) {
          case TLOGICAL:
            in_col = new FitsColumn<bool>(in_fp, ii + 1);
            out_col = new FitsColumn<bool>(out_fp, ii + 1);
            break;
          case TDOUBLE:
            in_col = new FitsColumn<double>(in_fp, ii + 1);
            out_col = new FitsColumn<double>(out_fp, ii + 1);
            break;
          case TFLOAT:
            in_col = new FitsColumn<float>(in_fp, ii + 1);
            out_col = new FitsColumn<float>(out_fp, ii + 1);
            break;
          case TLONG:
            in_col = new FitsColumn<long>(in_fp, ii + 1);
            out_col = new FitsColumn<long>(out_fp, ii + 1);
            break;
          case TSHORT:
            in_col = new FitsColumn<short>(in_fp, ii + 1);
            out_col = new FitsColumn<short>(out_fp, ii + 1);
            break;
          default: {
            delete in_col;
            fits_close_file(in_fp, &status);
            fits_close_file(out_fp, &status);
            std::ostringstream os;
            os << "Unexpected: TestColumn::copyDataFile encountered unknown type code " << type_code;
            throw std::logic_error(os.str());
            break;
          }
        }

        // Copy column from in to out.
        for (long jj = 0; jj < num_rows; ++jj) {
          out_col->copy(in_col, jj, jj);
        }
    
        // Clean up.
        delete out_col;
        delete in_col;
      }

      fits_compress_heap(out_fp, &status);
      if (0 != status) {
        setStatus(status);
        throw std::runtime_error("Unexpected: TestColumn::copyDataFile could not compress heap");
      }

    }

    fits_write_chksum(out_fp, &status);

    // Final clean up.
    fits_close_file(in_fp, &status);
    fits_close_file(out_fp, &status);

  }
}
