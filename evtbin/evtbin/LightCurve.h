/** \file LightCurve.h
    \brief Encapsulation of a Light curve, with methods to read/write using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_LightCurve_h
#define evtbin_LightCurve_h

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "evtbin/Hist1D.h"
#include "tip/Table.h"

namespace tip {
  class Header;
}

namespace evtbin {
  /** \class LightCurve
      \brief Encapsulation of a Light curve, with methods to read/write using tip.
  */
  class LightCurve {
    public:
      typedef std::vector<std::string> KeyCont_t;
      typedef std::map<std::string, std::string> KeyValuePairCont_t;

      /** \brief Create the light curve object.
          \param binner The binner used to create the histogram.
      */
      LightCurve(const Binner & binner);

      virtual ~LightCurve() throw();

      /** \brief Bin input from tip table.
          \param begin Table iterator pointing to the first record to be binned.
          \param end Table iterator pointing to one past the last record to be binned.
      */
      virtual void binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

      /** \brief Write standard OGIP light curve file.
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const;

      /** \brief Read values for all known keywords from the given header object.

           Any keywords missing from the header will simply be omitted in this object's
           container of key-value pairs.
           \param header The input header to scan for keywords.
      */
      void harvestKeywords(const tip::Header & header);

      /** \brief Update keywords from this light curve's current set of keywords in every extension of a file.
          \param file_name The name of the file to update.
      */
      void updateKeywords(const std::string & file_name) const;

      /** \brief Convert time object into a string representation suitable for storage in a date-like keyword.
          \param time The time to convert.
      */
      std::string formatDateKeyword(const time_t & time) const;
    private:
      mutable KeyValuePairCont_t m_key_value_pairs;
      KeyCont_t m_known_keys;
      std::string m_data_dir;
      Hist1D m_hist;
  };

}

#endif
