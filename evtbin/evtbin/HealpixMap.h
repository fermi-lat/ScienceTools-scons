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
      HealpixMap(const std::string & event_file, const std::string & event_table, const std::string & sc_file,const std::string & sc_table, const std::string & hpx_ordering_scheme, int hpx_order, bool hpx_ebin, const Binner & energy_binner, const Binner & ebounds, bool use_lb, const Gti & gti);

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

    private:
      std::string m_hpx_ordering_scheme;
      int m_hpx_order;
      bool m_hpx_ebin;
      bool m_use_lb;
      Binner * m_ebinner;
      Binner * m_ebounds;
      Cont_t m_data;
      HealpixBinner* m_hpx_binner;
      int m_emin;
      int m_emax;
  };

}

#endif
