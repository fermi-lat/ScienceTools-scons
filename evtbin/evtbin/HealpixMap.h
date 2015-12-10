/** \file HealpixMap.h
    \brief Encapsulation of a healpix map, with methods to read/write using tip.
    
*/
#ifndef evtbin_HealpixMap_h
#define evtbin_HealpixMap_h

#include <string>

#include "evtbin/DataProduct.h"
#include "evtbin/HealpixBinner.h"

namespace astro {
  class SkyProj;
}

namespace evtbin {

  /** \class HealpixMap
      \brief Encapsulation of a healpix map, with methods to read/write using tip.
  */
  class HealpixMap : public DataProduct {
    public:
    ///the internal representation of the energy vector of healpix vectors
    typedef std::vector< std::vector<double> > Cont_t;
      
      /** \brief Create the healpix map object.
      */
      HealpixMap(const std::string & event_file, const std::string & event_table, 
		 const std::string & sc_file, const std::string & sc_table, 
		 const std::string & hpx_ordering_scheme, int hpx_order, 
		 const std::string & region_string, 		  
		 bool hpx_ebin, const Binner & energy_binner, const Binner & ebounds, 
		 bool use_lb, const Gti & gti);

      /** \brief Create the healpix map object.
      */
      HealpixMap(const std::string & event_file, const std::string & event_table, 
		 const std::string & sc_file, const std::string & sc_table, 
		 const std::string & hpx_ordering_scheme, int hpx_nside, const nside_dummy dummy, 
		 const std::string & region_string, 	  
		 bool hpx_ebin, const Binner & energy_binner, const Binner & ebounds, 
		 bool use_lb, const Gti & gti);

      
      
      /** \brief Create the healpix map object.
      */
      HealpixMap(const std::string & healpixmap_file);

     
      virtual ~HealpixMap() throw();

      /** \brief Bin input from input file/files passed to the constructor.
      */
       virtual void binInput();

      /** \brief Bin input from tip table.
          \param begin Table iterator pointing to the first record to be binned.
          \param end Table iterator pointing to one past the last record to be binned.
      */
        virtual void binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

      //virtual void OpenInput(const std::string & event_file) const;


      /** \brief Write count map file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
       virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

       void writeSkymaps(const std::string & out_file) const;

       void fillBin(const double coord1, const double coord2, const double energy, double weight=1.);

       void readEbounds(const std::string & healpixmap_file);

       inline const std::vector<double> & energies() const {
	 return m_energies;
       }

       inline const HealpixBinner& hpx_binner() const {
	 return m_hpx_binner;
       }
       
       inline int nside() const {
	 return m_hpx_binner.healpix().Nside();
       }

       inline Healpix_Ordering_Scheme scheme() const {
	 return m_hpx_binner.healpix().Scheme();
       }

       inline bool isGalactic() const {
	 return m_hpx_binner.lb();
       }

     private:

      HealpixBinner m_hpx_binner;
      bool m_hpx_ebin;
      Binner * m_ebinner;
      Binner * m_ebounds;
      Cont_t m_data;
      
      int m_emin;
      int m_emax;
      std::vector<double> m_energies;
  };

}

#endif
