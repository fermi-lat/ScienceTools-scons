/** \file DataProduct.h
    \brief Base class for encapsulations of specific data products, with methods to read/write them using tip.
    \author James Peachey, HEASARC
*/
#ifndef evtbin_DataProduct_h
#define evtbin_DataProduct_h

#include <ctime>
#include <map>
#include <string>
#include <vector>

#include "evtbin/Gti.h"

#include "tip/KeyRecord.h"
#include "tip/Table.h"

namespace tip {
  class Header;
}

namespace evtbin {
  class Binner;
  class Hist;

  /** \class DataProduct
      \brief Base class for encapsulations of specific data products, with methods to read/write them using tip.
  */
  class DataProduct {
    public:
      typedef std::vector<std::string> KeyCont_t;
      typedef std::map<std::string, tip::KeyRecord> KeyValuePairCont_t;

      /** \brief Construct data product object from the given event and spacecraft file.
      */
      DataProduct(const std::string & event_file, const std::string & event_table, const Gti & gti);

      virtual ~DataProduct() throw();

      /** \brief Bin input from input file/files passed to the constructor.
      */
      virtual void binInput();

      /** \brief Bin input from tip table.
          \param begin Table iterator pointing to the first record to be binned.
          \param end Table iterator pointing to one past the last record to be binned.
      */
      virtual void binInput(tip::Table::ConstIterator begin, tip::Table::ConstIterator end);

      /** \brief Create a file, identifying the creator, and using the given template.
          \param creator The creator identifier, used to set the "CREATOR" keyword.
          \param out_file The output file name.
          \param fits_template The name of the template file.
      */
      void createFile(const std::string & creator, const std::string & out_file, const std::string & fits_template) const;

      /** \brief Write the standard output file(s).
          \param creator The value to write for the "CREATOR" keyword.
          \param out_file The output file name.
      */
      virtual void writeOutput(const std::string & creator, const std::string & out_file) const = 0;

      /** \brief Write the current GTI information to the given file's GTI extension. The extension must
          exist.
          \param out_file The output file name.
      */
      virtual void writeGti(const std::string & out_file) const;

      /** \brief Returns this object's current set of GTIs (read-only).
      */
      virtual const Gti & getGti() const;

      /** \brief Use the given (time) binner to modify the Gti by finding the overlap.
          Returns true if the Gti was actually changed by this operation, false if the
          Gti was unchanged.
          \param binner The binner.
      */
      virtual bool adjustGti(const Binner * binner);

      /** \brief Write ebounds extension.
          \param out_file The output file name.
          \param binner The binner used to write the output.
      */
      virtual void writeEbounds(const std::string & out_file, const Binner * binner) const;

      /** \brief Read values for all known keywords from the given file and extension.
           Any keywords missing from the header will simply be omitted in this object's
           container of key-value pairs.
      */
      void harvestKeywords(const std::string & file_name, const std::string & ext_name = "");

      /** \brief Read values for all known keywords from the given header object.
           Any keywords missing from the header will simply be omitted in this object's
           container of key-value pairs.
           \param header The input header to scan for keywords.
      */
      void harvestKeywords(const tip::Header & header);

      /** \brief Adjust and/or compute time-related key-value pairs for this data product. This method does not
                 directly modify keywords in any file. However, the modified values will be written if/when
                 this data product is written.

                 Keywords TSTART, TSTOP are modified only if the binner has a tighter range than the values in the original file.
                 Keywords EXPOSURE, ONTIME are computed from the spacecraft file.
          \param sc_file The spacecraft data file used to compute time keywords.
          \param binner Optional binner used to adjust TSTART and TSTOP if necessary. 
      */
      void adjustTimeKeywords(const std::string & sc_file, const Binner * binner = 0);

      /** \brief Update keywords in the given file, using this object's current set of key-value pairs. Every extension
                 of the file will be updated.
          \param file_name The name of the file to update.
      */
      void updateKeywords(const std::string & file_name) const;

      /** \brief Compute the total exposure.
          \param sc_file The name of the spacecraft data file to be used as input.
      */
      virtual double computeExposure(const std::string & sc_file) const;

      /** \brief Convert time object into a string representation suitable for storage in a date-like keyword.
          \param time The time to convert.
      */
      std::string formatDateKeyword(const time_t & time) const;

    protected:
      /** \brief Update a key-value pair, or add a new pair to the container of key-value pairs if it is not already present.
          \param name The name of the key-value pair to update.
          \param value The value to add to the key-value pair.
          \param comment The comment, which is only used if a new key-value pair needs to be added.
      */
      template <typename T>
      void updateKeyValue(const std::string & name, const T & value, const std::string & comment = "") const;

      mutable KeyValuePairCont_t m_key_value_pairs;
      KeyCont_t m_known_keys;
      std::string m_data_dir;
      std::string m_event_file;
      std::string m_event_table;
      Gti m_gti;
      Hist * m_hist_ptr;
  };

  template <typename T>
  inline void DataProduct::updateKeyValue(const std::string & name, const T & value, const std::string & comment) const {
    // See if it is present.
    KeyValuePairCont_t::iterator found = m_key_value_pairs.find(name);
    if (m_key_value_pairs.end() == found || found->second.empty()) {
      // Not present or blank, so add it.
      m_key_value_pairs[name] = tip::KeyRecord(name, value, comment);
    } else {
      // Already present, so only update it.
      m_key_value_pairs[name].setValue(value);
    }
  }

}

#endif
