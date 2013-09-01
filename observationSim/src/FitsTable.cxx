/**
 * @file FitsTable.cxx
 * @brief Implementation for class to write simple FITS binary tables.
 * This implementation is not terribly general, so we await a better
 * FITS handling class.
 * @author J. Chiang
 * $Header$
 */

#include "observationSim/FitsTable.h"

namespace observationSim {

FitsTable::FitsTable(const std::string filename, const std::string extName,
                     const std::vector<std::string> &columnNames, 
                     const std::vector<std::string> &format,
                     const std::vector<std::string> &unit) {

   m_extName = strdup(extName.c_str());

// The number of columns.
   m_tfields = static_cast<int>(columnNames.size());

// Ensure that the sizes of the vectors agree.
   if (format.size() != columnNames.size()) {
      std::cerr << "FitsTable::FitsTable:\n"
                << "The number of rows in format, " << format.size()
                << ", does not match the number in columnNames, " 
                << columnNames.size() << std::endl;
      exit(-1);
   }
   if (unit.size() != columnNames.size()) {
      std::cerr << "FitsTable::FitsTable\n"
                << "The number of rows in unit, " << unit.size()
                << ", does not match the number in columnNames, " 
                << columnNames.size() << std::endl;
      exit(-1);
   }

// Copy the string data.
   for (int i = 0; i < m_tfields && i < 100; i++) {
      m_ttype[i] = strdup(columnNames[i].c_str());
      m_tform[i] = strdup(format[i].c_str());
      m_tunit[i] = strdup(unit[i].c_str());
   }

// Create the file, deleting an existing file if necessary.
   remove(filename.c_str());
   int status = 0;
   fits_create_file(&m_fptr, filename.c_str(), &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      exit(-1);
   }
}

FitsTable::~FitsTable() {
   delete m_fptr;
   delete m_extName;
   for (int i = 0; i < m_tfields; i++) {
      delete m_ttype[i];
      delete m_tform[i];
      delete m_tunit[i];
   }
}

void FitsTable::writeTableData(const std::vector<std::vector<double> > &data) {
   
// Ensure that the size of the data vector is consistent in both dimesions.
   if (static_cast<int>(data.size()) != m_tfields) {
      std::cerr << "FitsTable::writeTableData:\n"
                << "The number of columns in data, " << data.size()
                << ", does not match the expected number of fields, " 
                << m_tfields << std::endl;
      exit(-1);
   }

   int status = 0;
   long nrows = static_cast<long>(data[0].size());
   fits_create_tbl(m_fptr, BINARY_TBL, nrows, m_tfields, m_ttype, m_tform,
                   m_tunit, m_extName, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      exit(-1);
   }

   int firstrow  = 1;  // first row in table to write
   int firstelem = 1;  // first element in row

// Loop over columns. Since the data in vectors are stored
// sequentially, one can pass the pointer to the first object as a C
// array.
   for (int i = 0; i < m_tfields; i++) {
      if (!strcmp(m_tform[i], "1I")) {
         std::vector<int> my_data(nrows);
         for (int j = 0; j < nrows; j++) {
            my_data[j] = static_cast<int>(data[i][j]);
         }
         fits_write_col(m_fptr, TINT, i+1, firstrow, firstelem, nrows, 
                        &my_data[0], &status);
         if (status != 0) {
            fits_report_error(stderr, status);
            exit(-1);
         }
      } else if (!strcmp(m_tform[i], "1E")) {
         std::vector<float> my_data(nrows);
         for (int j = 0; j < nrows; j++) {
            my_data[j] = static_cast<float>(data[i][j]);
         }
         fits_write_col(m_fptr, TFLOAT, i+1, firstrow, firstelem, nrows, 
                        &my_data[0], &status);
         if (status != 0) {
            fits_report_error(stderr, status);
            exit(-1);
         }
      } else if (!strcmp(m_tform[i], "1D")) {
         std::vector<double> my_data(nrows);
         for (int j = 0; j < nrows; j++) {
            my_data[j] = data[i][j];
         }
         fits_write_col(m_fptr, TDOUBLE, i+1, firstrow, firstelem, nrows, 
                        &my_data[0], &status);
         if (status != 0) {
            fits_report_error(stderr, status);
            exit(-1);
         }
      } // if (!strcmp(m_tform[i]...))
   } // i

   fits_close_file(m_fptr, &status);
   if (status != 0) {
      fits_report_error(stderr, status);
      exit(-1);
   }
}      

} // namespace observationSim
