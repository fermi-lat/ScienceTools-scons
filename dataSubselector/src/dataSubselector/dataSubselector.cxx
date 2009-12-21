/** 
 * @file dataSubselector.cxx
 * @brief Filter FT1 data.
 * @author J. Chiang
 *
 *  $Header$
 */

#include <algorithm>
#include <stdexcept>

#include "facilities/Util.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "dataSubselector/Gti.h"
#include "CutController.h"

using dataSubselector::CutController;
using dataSubselector::Gti;

/**
 * @class DataFilter
 * @author J. Chiang
 *
 * $Header$
 */

class DataFilter : public st_app::StApp {
public:
   DataFilter() : st_app::StApp(), 
                  m_pars(st_app::StApp::getParGroup("gtselect")),
                  m_tstart(0), m_tstop(0) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in DataFilter constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~DataFilter() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

   virtual void banner() const;

   void promptForParameters();

private:

   st_app::AppParGroup & m_pars;

   std::vector<std::string> m_inputFiles;
   std::string m_outputFile;

   mutable double m_tstart, m_tstop;

   void copyTable(const std::string & extension,
                  CutController * cutController=0) const;

   void copyGtis() const;

   void writeDateKeywords() const;

   static std::string s_cvs_id;
};

std::string DataFilter::s_cvs_id("$Name$");

st_app::StAppFactory<DataFilter> myAppFactory("gtselect");

void DataFilter::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void DataFilter::promptForParameters() {
   m_pars.Prompt("infile");
   m_pars.Prompt("outfile");
   m_pars.Prompt("ra");
   m_pars.Prompt("dec");
   m_pars.Prompt("rad");
   try {
      m_pars.Prompt("tmin");
      m_pars.Prompt("tmax");
   } catch (const hoops::Hexception &) {
      m_pars["tmin"] = static_cast<float>(0);
      m_pars["tmax"] = static_cast<float>(0);
   }
   m_pars.Prompt("emin");
   m_pars.Prompt("emax");
   m_pars.Prompt("zmax");
   m_pars.Save();
}

void DataFilter::run() {
   m_pars.Prompt();
   m_pars.Save();

//    promptForParameters();

   std::string evtable = m_pars["evtable"];

   std::string inputFile = m_pars["infile"];
   st_facilities::Util::resolve_fits_files(inputFile, m_inputFiles);

   std::string outputFile = m_pars["outfile"];
   m_outputFile = outputFile;
   facilities::Util::expandEnvVar(&m_outputFile);
   bool clobber = m_pars["clobber"];
   st_stream::StreamFormatter formatter("DataFilter", "run", 2);
   if (!clobber && st_facilities::Util::fileExists(m_outputFile)) {
      formatter.err() << "Output file, " << outputFile << ", already exists,\n"
                      << "and you have specified 'clobber' as 'no'.\n"
                      << "Please provide a different file name." 
                      << std::endl;
      std::exit(1);
   } 

   if (m_inputFiles.size() > 1) {
      tip::IFileSvc::instance().createFile(m_outputFile, m_inputFiles.front());
   }

   CutController * cuts = 
      CutController::instance(m_pars, m_inputFiles, evtable);
   copyTable(evtable, cuts);
   copyGtis();
   cuts->updateGti(m_outputFile);
   CutController::delete_instance();

   double tmin, tmax;
   tmin = m_pars["tmin"];
   tmax = m_pars["tmax"];
   if (tmin != 0 || tmax != 0 || m_inputFiles.size() > 1) {
      if (tmin != 0 || tmax != 0) {
         m_tstart = std::max(m_tstart, tmin);
         m_tstop = std::min(m_tstop, tmax);
      }
      writeDateKeywords();
   }

   st_facilities::FitsUtil::writeChecksums(m_outputFile);

   formatter.info() << "Done." << std::endl;
}

void DataFilter::writeDateKeywords() const {
   tip::Image * phdu(tip::IFileSvc::instance().editImage(m_outputFile, ""));
   st_facilities::Util::writeDateKeywords(phdu, m_tstart, m_tstop, false);
   delete phdu;

   std::string evtable = m_pars["evtable"];
   tip::Table * table
      = tip::IFileSvc::instance().editTable(m_outputFile, evtable);
   st_facilities::Util::writeDateKeywords(table, m_tstart, m_tstop);
   delete table;

   table = tip::IFileSvc::instance().editTable(m_outputFile, "GTI");
   st_facilities::Util::writeDateKeywords(table, m_tstart, m_tstop);
   delete table;
}

void DataFilter::copyTable(const std::string & extension,
                           CutController * cuts) const {
   std::string filterString("");
   if (cuts) {
      filterString = cuts->filterString();
      st_stream::StreamFormatter formatter("DataFilter", "copyTable", 3);
      formatter.info() << "Applying filter string: " 
                       << filterString << std::endl;
   }

   if (m_inputFiles.size() == 1) { // use cfitsio directly
      st_facilities::FitsUtil::fcopy(m_inputFiles.at(0), m_outputFile,
                                     extension, filterString, 
                                     m_pars["clobber"]);
// Get tstart, tstop
      const tip::Table * inputTable 
         = tip::IFileSvc::instance().readTable(m_inputFiles.front(), 
                                               extension, filterString);
      const tip::Header & header(inputTable->getHeader());
      header["TSTART"].get(m_tstart);
      header["TSTOP"].get(m_tstop);
      delete inputTable;
   } else { // handle multiple input files using tip
      std::vector<std::string>::const_iterator infile(m_inputFiles.begin());
      tip::Index_t nrows(0);
      for ( ; infile != m_inputFiles.end(); ++infile) {
         const tip::Table * inputTable 
            = tip::IFileSvc::instance().readTable(*infile, extension,
                                                  filterString);
         nrows += inputTable->getNumRecords();
// Get tstart, tstop for this file, treating the first file as a
// special case to initialize the values.
         const tip::Header & header(inputTable->getHeader());
         double tstart, tstop;
         header["TSTART"].get(tstart);
         header["TSTOP"].get(tstop);
         if (infile == m_inputFiles.begin()) {
            m_tstart = tstart;
            m_tstop = tstop;
         } else {
            if (tstart < m_tstart) {
               m_tstart = tstart;
            }
            if (tstop > m_tstop) {
               m_tstop = tstop;
            }
         }
         delete inputTable;
      }

      tip::Table * outputTable 
         = tip::IFileSvc::instance().editTable(m_outputFile, extension);
      outputTable->setNumRecords(nrows);

      tip::Table::Iterator outputIt = outputTable->begin();
      tip::Table::Record & output = *outputIt;

      tip::Index_t npts(0);
      for (infile=m_inputFiles.begin(); infile != m_inputFiles.end();
           ++infile) {
         const tip::Table * inputTable 
            = tip::IFileSvc::instance().readTable(*infile, extension, 
                                                  filterString);
         tip::Table::ConstIterator inputIt = inputTable->begin();
         tip::ConstTableRecord & input = *inputIt;
         for (; inputIt != inputTable->end(); ++inputIt) {
            output = input;
            ++outputIt;
            npts++;
         }
         delete inputTable;
      }

// Resize output table to account for filtered rows.
      outputTable->setNumRecords(npts);
      delete outputTable;
   }

// (Re)open outputTable and write keywords
   tip::Table * outputTable 
      = tip::IFileSvc::instance().editTable(m_outputFile, extension);

   if (cuts) {
      cuts->writeDssKeywords(outputTable->getHeader());
   }

   outputTable->getHeader().addHistory("Filter string: " + filterString);

   delete outputTable;
}

void DataFilter::copyGtis() const {
   Gti gti(m_inputFiles.front());
   for (size_t i(1); i < m_inputFiles.size(); i++) {
      Gti my_gti(m_inputFiles.at(i));
      gti |= my_gti;
   }
   gti.writeExtension(m_outputFile);
}
