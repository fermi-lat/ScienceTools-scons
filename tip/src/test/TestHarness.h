/** \file TestHarness.h
    \brief Declaration of common test reporting/utility class.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestHarness_h
#define tip_TestHarness_h

#include <sstream>
#include <stdexcept>
#include <string>

namespace tip {

  /** \class TestHarness
      \brief Common test reporting/utility class.
  */
  class TestHarness {
    public:
      /** \class Ignore
          \brief Straw man exception class whose raison d'etre is to be ignored.
      */
      class Ignore : public std::exception {};

      /** \brief Construct Test harness.
      */
      TestHarness();

      virtual ~TestHarness() throw();

      /** \brief Perform the detailed test needed by the subobject.
          \param status Error status inherited from caller.
      */
      virtual int test(int status) = 0;

      /** \brief Report an action as expected behavior.
          \param context Specific information about the action which occurred.
          \param x An exception, if any is relevant in this context.
      */
      void ReportExpected(const std::string & context, const std::exception & x = Ignore()) const;

      /** \brief Report an action as unexpected behavior.
          \param context Specific information about the action which occurred.
          \param x An exception, if any is relevant in this context.
      */
      void ReportUnexpected(const std::string & context, const std::exception & x = Ignore()) const;

      /** \brief Report an action as unexpected behavior.
          \param context Specific information about the action which occurred.
          \param x An exception, if any is relevant in this context.
      */
      void ReportWarning(const std::string & msg) const;

      /** \brief Convert to a string any type which has streaming behavior.
          \param value Object to convert. Type is that of the template parameter.
      */
      template <typename T>
      std::string ToString(const T & value) {
        std::ostringstream os;
        os << value;
        return os.str();
      }

      /** \brief Get status of tests.
      */
      int getStatus() const;

      /** \brief Set status of tests.
          \param status New status. If this test object already has non-0 status, it will not be changed.
      */
      void setStatus(int status) const;

      /** \brief Get name of directory containing test data.
      */
      const std::string & getDataDir() const;

    private:
      mutable std::string m_data_dir;
      mutable int m_status;
  };

}

#endif
