/** \file HealpixMap.cxx
    \brief Encapsulation of a healpix map, with methods to read/write using tip.
   
*/
#include <algorithm>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "evtbin/LinearBinner.h"
#include "evtbin/HealpixMap.h"
#include "evtbin/HealpixBinner.h"

#include "facilities/commonUtilities.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

namespace evtbin {
     
   HealpixMap::HealpixMap(const std::string & event_file, const std::string & event_table, const std::string & sc_file,
			  const std::string & sc_table, const std::string & hpx_ordering_scheme, int hpx_order, bool hpx_ebin,
			  const Binner & energy_binner, const Binner & ebounds, bool use_lb, const Gti & gti)
  : DataProduct(event_file, event_table, gti), 
    m_ebinner(energy_binner.clone()), 
    m_hpx_ordering_scheme(hpx_ordering_scheme), m_hpx_order(hpx_order), m_hpx_ebin(hpx_ebin), m_use_lb(use_lb), m_ebounds(ebounds.clone()), m_emin(0.),m_emax(0.) {    
     m_hpx_binner = new HealpixBinner(hpx_ordering_scheme, hpx_order, use_lb);
     // Set initial size of data array : m_data sized to the number of energy bin
     // m_ebinner->getNumBins() returns 0 if no energy binning is requested, which needs to default to 1 bin.
     m_data.resize(m_ebinner->getNumBins()?m_ebinner->getNumBins():1) ;
     for (Cont_t::iterator itor = m_data.begin(); itor != m_data.end(); ++itor) {
       //each element of m_data sized to the number of healpix requested.
       itor->resize(m_hpx_binner->getNumBins(), 0);
     }

    // Collect any/all needed keywords from the primary extension.
    harvestKeywords(m_event_file_cont);

    // Collect any/all needed keywords from the GTI extension.
    // But do not fail if GTI isn't there. This is for GBM headers.
    try {
      harvestKeywords(m_event_file_cont, "GTI");
    } catch (...){}

    // Collect any/all needed keywords from the ebounds extension.
    // But do not fail if ebounds isn't there.  This is for GBM headers.
    try {
      harvestKeywords(m_event_file_cont, "EBOUNDS");
    } catch (...){}

    // Collect any/all needed keywords from the events extension.
    harvestKeywords(m_event_file_cont, m_event_table);

    // Correct time keywords.
    adjustTimeKeywords(sc_file, sc_table);     
   }


//Destructeur
  HealpixMap::~HealpixMap() throw() 
  {
    delete m_ebinner;
    delete m_hpx_binner;
  }

 
  void HealpixMap::binInput() {
    DataProduct::binInput();
  }

void HealpixMap::binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end) {
     // From each binner, get the name of its field.
    std::string energy_field = m_ebinner->getName();
    std::string healpix_field = m_hpx_binner->getName();

    //initialize m_emin
    m_emin=(*begin)[energy_field].get();
    // Fill histogram, converting each coord to pix number on the fly:
    for (tip::Table::ConstIterator itor = begin; itor != end; ++itor)
      {
	// Extract the data from each record.
	double energy = (*itor)[energy_field].get();   //get the data in col ENERGY
	double ra = (*itor)["RA"].get();
	double dec = (*itor)["DEC"].get();
	double l = (*itor)["L"].get();
	double b = (*itor)["B"].get();
	
	// There is no easy way to make use of Hist2D::fillBin in the
	// case of HEALPIX spatial binner, so we clone the loop here
	// deal with (l,b) or (ra,dec)?
	if (m_use_lb) fillBin(l, b, energy);
        else       fillBin(ra, dec, energy);

	//this is bookkeeping for EBOUNDS in case of no ebinning request
	m_emax=energy>m_emax?energy:m_emax;
	m_emin=energy<m_emin?energy:m_emin;
    }//end for
} //end binInput

  void HealpixMap::writeOutput(const std::string & creator, const std::string & out_file) const {
    // Standard file creation from base class.
    createFile(creator, out_file, facilities::commonUtilities::joinPath(m_data_dir, "LatHealpixTemplate"));

    //access SKYMAPS extension
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SKYMAPS"));
    tip::Header & header(output_table->getHeader());
   

    // Write the SKYMAPS  extension
    writeSkymaps(out_file);
        
    // Write the history that came from the skymaps extension.
    writeHistory(*output_table, "EVENTS"); 

    // Write DSS keywords to preserve cut information.
    writeDssKeywords(header);

    // Write the EBOUNDS extension.
    if(m_hpx_ebin){ writeEbounds(out_file, m_ebounds);}
    else {
      writeEbounds(out_file, &LinearBinner(m_emin,m_emax,m_emax-m_emin));}
    // Write the GTI extension.
    writeGti(out_file);
  
  }  
  
  void HealpixMap::writeSkymaps(const std::string & out_file) const {
    //Open the skymap extension
    std::auto_ptr<tip::Table> output_table(tip::IFileSvc::instance().editTable(out_file, "SKYMAPS"));
    //resize the table to have as many records as there are healpixels
    output_table->setNumRecords(m_hpx_binner->getNumBins());
    
    //now iterate over the healpixels
    for (long e_index = 0; e_index != (m_ebinner->getNumBins()==0?1:m_ebinner->getNumBins()); ++e_index) {
      std::ostringstream e_channel;
      e_channel<<"CHANNEL"<<e_index+1;
      //create new column
      output_table->appendField(e_channel.str(), std::string("D"));
      //loop over rows and fill healpix values in
      tip::Table::Iterator table_itor = output_table->begin();
      for(long hpx_index = 0;hpx_index != m_hpx_binner->getNumBins();++hpx_index,++table_itor) {
	(*table_itor)[e_channel.str()].set(m_data[e_index][hpx_index]);
      }
    }
    std::string coordsys;
    if (m_use_lb) {
      coordsys = "GAL";
    } else {
      coordsys = "EQU"; 
    }
    int long Nside=pow((long double)2,m_hpx_order);
    int long lastpix=12*Nside*Nside-1;
   // int long nbrbins= nbchannel;
    
    tip::Header & header(output_table->getHeader());
    header["ORDERING"].set(m_hpx_ordering_scheme); 
    header["ORDER"].set(m_hpx_order);
    header["COORDSYS"].set(coordsys);
    header["NSIDE"].set(Nside); 
    header["FIRSTPIX"].set(0); 
    header["LASTPIX"].set(lastpix);
    //header["NAXIS2"].set(lastpix+1);
    
}

  void HealpixMap::fillBin(const double coord1, const double coord2, const double energy, double weight)
  {
    //computeIndex returns -1 if m_ebinner has 0 bin, 
    //which is the case if no energy binning is requested by the user.
    long index1 = m_ebinner->getNumBins()>1?m_ebinner->computeIndex(energy):0;
    long index2 = m_hpx_binner->computeIndex(coord1,coord2);
   
    // Make sure indices are valid:
    if (0 <= index1 && 0 <= index2) {
      m_data[index1][index2] += weight;
    }
    
  }

}

