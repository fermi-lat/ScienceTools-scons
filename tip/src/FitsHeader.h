#ifndef tip_FitsHeader_h
#define tip_FitsHeader_h

#include <string>

#include "fitsio.h"

#include "FitsPrimProps.h"
#include "tip/Header.h"

namespace tip {

  class FitsHeader : public Header {
    public:
      FitsHeader(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "", bool read_only = true);

      virtual ~FitsHeader();

      /** \brief Open the FITS file and return Cfitsio's fitsfile pointer.
      */
      void open();

      /** \brief Close the FITS file.
          \param status The status to use when closing the file. Note this is not a reference!
      */
      void close(int status = 0);

      fitsfile * getFp() const { return m_fp; }

      bool isTable() const { return m_is_table; }

      bool readOnly() const { return m_read_only; }

      /** \brief Get a keyword from this header data object.
          \param name The name of the keyword to get from the header data object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const;
      virtual void getKeyword(const std::string & name, double & value) const;
      virtual void getKeyword(const std::string & name, float & value) const;
      virtual void getKeyword(const std::string & name, char & value) const;
      virtual void getKeyword(const std::string & name, signed char & value) const;
      virtual void getKeyword(const std::string & name, signed short & value) const;
      virtual void getKeyword(const std::string & name, signed int & value) const;
      virtual void getKeyword(const std::string & name, signed long & value) const;
      virtual void getKeyword(const std::string & name, unsigned char & value) const;
      virtual void getKeyword(const std::string & name, unsigned short & value) const;
      virtual void getKeyword(const std::string & name, unsigned int & value) const;
      virtual void getKeyword(const std::string & name, unsigned long & value) const;
      virtual void getKeyword(const std::string & name, std::string & value) const;

      virtual void getKeyRecord(const std::string & name, std::string & value) const;

      /** \brief Set a keyword in this header data object.
          \param name The name of the keyword to set in the header data object.
          \param value The input value of the keyword.
      */
      virtual void setKeyword(const std::string & name, const bool & value);
      virtual void setKeyword(const std::string & name, const double & value);
      virtual void setKeyword(const std::string & name, const float & value);
      virtual void setKeyword(const std::string & name, const char & value);
      virtual void setKeyword(const std::string & name, const signed char & value);
      virtual void setKeyword(const std::string & name, const signed short & value);
      virtual void setKeyword(const std::string & name, const signed int & value);
      virtual void setKeyword(const std::string & name, const signed long & value);
      virtual void setKeyword(const std::string & name, const unsigned char & value);
      virtual void setKeyword(const std::string & name, const unsigned short & value);
      virtual void setKeyword(const std::string & name, const unsigned int & value);
      virtual void setKeyword(const std::string & name, const unsigned long & value);
      virtual void setKeyword(const std::string & name, const std::string & value);
      virtual void setKeyword(const std::string & name, const char * const & value);

      virtual void setKeyRecord(const std::string & name, const std::string & value);

      /** \brief Return the name of the particular column implementation (subclass identifier).
      */
      virtual const std::string implementation() const { return "FITS"; }

      /** \brief Return the comment associated with the given keyword.
          \param name The name of the keyword.
      */
      virtual std::string getKeyComment(const std::string & name) const;

      /** \brief Set comment associated with the given keyword.
          \param name The name of the keyword.
          \param comment The new comment of the keyword.
      */
      virtual void setKeyComment(const std::string & name, const std::string & comment);

      /** \brief Return the unit associated with the given keyword.
          \param name The name of the keyword.
      */
      virtual std::string getKeyUnit(const std::string & name) const;

      /** \brief Set unit associated with the given keyword.
          \param name The name of the keyword.
          \param unit The new unit of the keyword.
      */
      virtual void setKeyUnit(const std::string & name, const std::string & unit);

    private:
      /** \brief Templated function which can get keywords from a FITS table, converted to any data type.
          \param name The name of the keyword.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      /** \brief Templated function which can set keywords in a FITS table.
          \param name The name of the keyword.
          \param value The value to be written.
      */
      template <typename T>
      void setKeywordGeneric(const std::string & name, const T & value);

      std::string formatWhat(const std::string & msg) const;

      std::string m_file_name;
      std::string m_ext_name;
      std::string m_filter;
      fitsfile * m_fp;
      bool m_is_table;
      bool m_read_only;
  };

  // Getting keywords.
  template <typename T>
  inline void FitsHeader::getKeywordGeneric(const std::string & name, T & value) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &value, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
  }

  // Getting keywords as bool is a special case because Cfitsio treats them as ints.
  template <>
  inline void FitsHeader::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
    value = tmp;
  }

  // Getting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsHeader::getKeywordGeneric<std::string>(const std::string & name, std::string & value) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
    value = tmp;
  }

  // Setting keywords.
  template <typename T>
  inline void FitsHeader::setKeywordGeneric(const std::string & name, const T & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    T tmp = value;
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as bool is a special case because Cfitsio treats them as ints.
  template <>
  inline void FitsHeader::setKeywordGeneric<bool>(const std::string & name, const bool & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = value;
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsHeader::setKeywordGeneric<std::string>(const std::string & name, const std::string & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    strncpy(tmp, value.c_str(), FLEN_KEYWORD - 1);
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsHeader::setKeywordGeneric<const char *>(const std::string & name, const char * const & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<const char *>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    strncpy(tmp, value, FLEN_KEYWORD - 1);
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  inline void FitsHeader::getKeyword(const std::string & name, bool & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, double & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, float & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, char & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, signed char & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, signed short & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, signed int & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, signed long & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, unsigned char & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, unsigned short & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, unsigned int & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, unsigned long & value) const { getKeywordGeneric(name, value); }
  inline void FitsHeader::getKeyword(const std::string & name, std::string & value) const { getKeywordGeneric(name, value); }

  inline void FitsHeader::getKeyRecord(const std::string & name, std::string & record) const {
    int status = 0;
    char tmp[FLEN_CARD];
    fits_read_card(m_fp, const_cast<char *>(name.c_str()), tmp, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read key record \"") + name + '"'));
    record = tmp;
  }

  inline void FitsHeader::setKeyword(const std::string & name, const bool & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const double & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const float & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const char & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const signed char & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const signed short & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const signed int & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const signed long & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const unsigned char & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const unsigned short & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const unsigned int & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const unsigned long & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const std::string & value) { setKeywordGeneric(name, value); }
  inline void FitsHeader::setKeyword(const std::string & name, const char * const & value) { setKeywordGeneric(name, value); }

  inline void FitsHeader::setKeyRecord(const std::string & name, const std::string & record) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write key record\"") + name + "\"; object is not writable"));
    int status = 0;
    char tmp[FLEN_CARD];
    strncpy(tmp, record.c_str(), FLEN_CARD - 1);
    fits_update_card(m_fp, const_cast<char *>(name.c_str()), tmp, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write key record\"") + name + '"'));
  }

}

#endif
