/**
 * @file gtmaketime.cxx
 * @brief Create a GTI extension based on a filter expression applied
 * to a spacecraft data file and merge with the existing GTI in the
 * event data file.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"
#include "dataSubselector/GtiCut.h"
#include "dataSubselector/RangeCut.h"
#include "dataSubselector/SkyConeCut.h"

/**
 * @class MakeTime
 * @author J. Chiang
 *
 */

class MakeTime : public st_app::StApp {
public:
   MakeTime() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("gtmktime")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in Maketime constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~MakeTime() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

   virtual void banner() const;

private:
   
   st_app::AppParGroup & m_pars;
   dataSubselector::Gti m_gti;
   double m_tmin;
   double m_tmax;
   std::string m_evfile;
   std::string m_outfile;

   void check_outfile();
   void findTimeLims();
   std::string roiZenAngleCut();
   void createGti();
   void mergeGtis();
   void makeUserGti(std::vector<const dataSubselector::GtiCut*>&gtiCuts) const;
   void writeGtiFile(const std::string & gtifile) const;
   void copyTable() const;
   void updateKeywords() const;

   static std::string s_cvs_id;
};

std::string MakeTime::s_cvs_id("$Name$");

st_app::StAppFactory<MakeTime> myAppFactory("gtmktime");

void MakeTime::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void MakeTime::run() {
   m_pars.Prompt();
   m_pars.Save();
   check_outfile();
   findTimeLims();
   createGti();
   mergeGtis();
   copyTable();
   updateKeywords();
}

void MakeTime::check_outfile() {
   st_stream::StreamFormatter formatter("MakeTime", "run", 2);
   bool clobber = m_pars["clobber"];
   std::string outfile = m_pars["outfile"];
   m_outfile = outfile;
   if (outfile == "") {
      formatter.err() << "Please specify an output file name." 
                      << std::endl;
      std::exit(1);
   }
   if (!clobber && st_facilities::Util::fileExists(outfile)) {
      formatter.err() << "Output file " << outfile 
                      << " exists and clobber is set to 'no'.  Exiting."
                      << std::endl;
      std::exit(1);
   }
}

void MakeTime::findTimeLims() {
   std::string evfile = m_pars["evfile"];
   std::string evtable = m_pars["evtable"];
   const tip::Table * evTable
      = tip::IFileSvc::instance().readTable(evfile, evtable);
   const tip::Header & header(evTable->getHeader());
   header["TSTART"].get(m_tmin);
   header["TSTOP"].get(m_tmax);
}

std::string MakeTime::roiZenAngleCut() {
   bool apply_roi_cut = m_pars["roicut"];
   if (!apply_roi_cut) {
      return "";
   }
   std::string evfile = m_pars["evfile"];
   std::string evtable = m_pars["evtable"];
   bool checkColumns = m_pars["apply_filter"];
   dataSubselector::Cuts cuts(evfile, evtable, checkColumns);

   double zmax(180);
   for (size_t i(0); i < cuts.size(); i++) {
      if (cuts[i].type() == "range") {
         const dataSubselector::RangeCut & rangeCut
            = dynamic_cast<dataSubselector::RangeCut &>(const_cast<dataSubselector::CutBase &>(cuts[i]));
         if (rangeCut.colname() == "ZENITH_ANGLE") {
            zmax = rangeCut.maxVal();
         }
      }
   }

   if (zmax < 180) {
      for (size_t i(0); i < cuts.size(); i++) {
         if (cuts[i].type() == "SkyCone") {
            const dataSubselector::SkyConeCut & skyConeCut
               = dynamic_cast<dataSubselector::SkyConeCut &>(const_cast<dataSubselector::CutBase &>(cuts[i]));
            
            std::ostringstream filter;
            filter << " && angsep(RA_ZENITH,DEC_ZENITH," 
                   << skyConeCut.ra() << "," << skyConeCut.dec()
                   << ") < " << zmax - skyConeCut.radius();
            return filter.str();
         }
      }
   }
   st_stream::StreamFormatter formatter("MakeTime", "roiZenAngleCut", 2);
   formatter.info() << "DSS keywords required for ROI-based zenith angle cut"
                    << "\nare not present in the FT1 file." << std::endl;
   return "";
}

void MakeTime::createGti() {
   std::string scfile = m_pars["scfile"];
   std::vector<std::string> scfiles;
   st_facilities::Util::resolve_fits_files(scfile, scfiles);
   std::string sctable = m_pars["sctable"];
   std::string filter = m_pars["filter"];

   filter += roiZenAngleCut();

   std::ostringstream event_time_range;
   event_time_range << std::setprecision(20);
   event_time_range << " && (START >= " << m_tmin
                    << ") && (STOP <= " << m_tmax << ")";
   filter += event_time_range.str();

   st_stream::StreamFormatter formatter("MakeTime", "createGti", 3);
   formatter.info() << "Applying GTI filter:\n" << filter << std::endl;

   for (size_t i = 0; i < scfiles.size(); i++) {
      std::auto_ptr<const tip::Table> 
         in_table(tip::IFileSvc::instance().readTable(scfiles.at(i), 
                                                      sctable, filter));

      if (in_table->getNumRecords() == 0) {
         std::ostringstream message;
         message << "Zero rows returned from FT2 file for this filter:\n"
                 << filter;
         throw std::runtime_error(message.str());
      }

      tip::Table::ConstIterator input = in_table->begin();
      tip::ConstTableRecord & in = *input;

      double start_time;
      double stop_time;
      std::vector<double> tstart;
      std::vector<double> tstop;
// Initialize arrays with the first interval that ends after the first
// event time, m_tmin
      for (; input != in_table->end(); ++input) {
         in["START"].get(start_time);
         in["STOP"].get(stop_time);
         if (stop_time > m_tmin) {
            tstart.push_back(start_time);
            tstop.push_back(stop_time);
            break;
         }
      }
      if (input != in_table->end()) {
// Gather remaining intervals, consolidating adjacent ones.
         for (; input != in_table->end(); ++input) {
            in["START"].get(start_time);
            in["STOP"].get(stop_time);
            if (start_time > m_tmax) { // break out if past end of evfile
               break;
            }
            if (start_time == tstop.back()) {
               tstop.back() = stop_time;
            } else {
               tstart.push_back(start_time);
               tstop.push_back(stop_time);
            }
         }
      }
// Insert each contiguous interval in the Gti object
      for (size_t i(0); i < tstart.size(); i++) {
         dataSubselector::Gti gti;
         gti.insertInterval(tstart.at(i), tstop.at(i));
         m_gti = m_gti | gti;
      }
   }
}

void MakeTime::mergeGtis() {
   std::string evfile = m_pars["evfile"];
   m_evfile = evfile;
   std::string evtable = m_pars["evtable"];
   
   bool checkColumns = m_pars["apply_filter"];
   dataSubselector::Cuts cuts(evfile, evtable, checkColumns);

   std::vector<const dataSubselector::GtiCut *> gtiCuts;

   bool overwrite = m_pars["overwrite"];
   if (overwrite) {
      makeUserGti(gtiCuts);
   } else {
      cuts.getGtiCuts(gtiCuts);
   }

   for (size_t i = 0; i < gtiCuts.size(); i++) {
      m_gti = m_gti & gtiCuts.at(i)->gti();
   }
}

void MakeTime::
makeUserGti(std::vector<const dataSubselector::GtiCut *> & gtiCuts) const {
   double tstart = m_pars["tstart"];
   double tstop = m_pars["tstop"];
   bool useHeader = m_pars["header_obstimes"];
   std::string extension = m_pars["evtable"];
   if (useHeader) {
      const tip::Table * evTable 
         = tip::IFileSvc::instance().readTable(m_evfile, extension);
      const tip::Header & header(evTable->getHeader());
      header["TSTART"].get(tstart);
      header["TSTOP"].get(tstop);
   }
   dataSubselector::Gti myGti;
   myGti.insertInterval(tstart, tstop);
   gtiCuts.clear();
   gtiCuts.push_back(new dataSubselector::GtiCut(myGti));
}

void MakeTime::writeGtiFile(const std::string & gtifile) const {
   bool clobber;
   tip::IFileSvc::instance().createFile(gtifile, m_evfile, clobber=true);
   m_gti.writeExtension(gtifile);
}

void MakeTime::copyTable() const {
   std::string gtifile = m_pars["gtifile"];
   writeGtiFile(gtifile);

   std::string extension = m_pars["evtable"];
   std::string filterString("gtifilter(\"" + gtifile + "\")");

   st_facilities::FitsUtil::fcopy(m_evfile, m_outfile, extension,
                                  filterString, m_pars["clobber"]);
   m_gti.writeExtension(m_outfile);

   st_facilities::FitsUtil::writeChecksums(m_outfile);
   std::remove(gtifile.c_str());
}

void MakeTime::updateKeywords() const {
   tip::Image * my_image(tip::IFileSvc::instance().editImage(m_outfile, ""));
   std::string filename(facilities::Util::basename(m_outfile));
   my_image->getHeader().setKeyword("FILENAME", filename);
   delete my_image;
}

/** 
 * June 18, 2007:  optimization of GTI accumulation in run() method.

Timing before optimization: 

ki-rh2[jchiang] time ../../rhel4_gcc34/gtmktime.exe 
Spacecraft data file [test_scData.fits] : 
Filter expression [IN_SAA!=T] : 
Event data file [test_events_0000.fits] : 
Output event file name [filtered.fits] : 
229.168u 0.182s 3:57.25 96.6%   0+0k 0+0io 0pf+0w

After optimization to createGti():
ki-rh2[jchiang] time ../../rhel4_gcc34/gtmktime.exe
Spacecraft data file [test_scData.fits] : 
Filter expression [IN_SAA!=T] : 
Event data file [test_events_0000.fits] : 
Output event file name [filtered.fits] : filtered_new.fits
4.693u 0.173s 0:10.87 44.7%     0+0k 0+0io 0pf+0w

 */
