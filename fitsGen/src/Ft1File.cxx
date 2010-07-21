/**
 * @file Ft1File.cxx
 * @brief Implementation of FT1 file abstraction.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>
#include <string>

#include "tip/IFileSvc.h"
#include "tip/Image.h"

#include "fitsGen/Ft1File.h"

namespace fitsGen {

Ft1File::Ft1File(const std::string & outfile, long nrows, 
                 const std::string & extname,
                 const std::string & templateFile)
   : FtFileBase(outfile, nrows) {
   init(templateFile, extname);
}

Ft1File::~Ft1File() {
   close();
}

void Ft1File::close() {
//    verifyObsTimes();

   if (m_table) {
      writeDateKeywords(m_table, m_startTime, m_stopTime,
                        true, s_missionStart);
      delete m_table;
      m_table = 0;

      tip::IFileSvc & fileSvc(tip::IFileSvc::instance());

      try {
         tip::Table * gtiTable(fileSvc.editTable(m_outfile, "GTI"));
         writeDateKeywords(gtiTable, m_startTime,
                           m_stopTime, true, s_missionStart);
         delete gtiTable;
      } catch (...) {
      }

      tip::Image * phdu(fileSvc.editImage(m_outfile, ""));
      writeDateKeywords(phdu, m_startTime, m_stopTime,
                        false, s_missionStart);
      delete phdu;
   }
}

void Ft1File::verifyObsTimes() {
// Infer start and stop times from events if necessary.  The entries
// may not be ordered, so we need to loop over the entire dataset.
   double start, stop;
   if (m_startTime < 0 || m_stopTime < 0) {
      m_it = begin();
      start = (*m_it)["TIME"].get();
      stop = start;
      for ( ; m_it != end(); ++m_it) {
         double time((*m_it)["TIME"].get());
         if (time < start) {
            start = time;
         } else if (time > stop) {
            stop = time;
         }
      }
   }
   if (m_startTime < 0) {
      m_startTime = start - 1;
   }
   if (m_stopTime < 0) {
      m_stopTime = stop + 1;
   }
}

} // namespace fitsGen
