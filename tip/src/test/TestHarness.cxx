/** \file TestHarness.cxx
    \brief Definition of common test reporting/utility class.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <string>
#include <typeinfo>

#include "facilities/commonUtilities.h"

#include "TestHarness.h"

namespace tip {

  TestHarness::TestHarness(): m_data_dir(), m_status(0) {}

  TestHarness::~TestHarness() throw() {}

  void TestHarness::ReportExpected(const std::string & context, const std::exception & x) const {
#if 1
    std::cerr << "Expected behavior: " << context;
    if (typeid(x) != typeid(Ignore)) {
      std::cerr << "\n\twhat() == ";
      const char * what = x.what();
      if (0 != what) std::cerr << what;
    }
    std::cerr << "\n" << std::endl;
#endif
  }

  void TestHarness::ReportUnexpected(const std::string & context, const std::exception & x) const {
    setStatus(1);
    std::cerr << "Unexpected behavior: " << context;
    if (typeid(x) != typeid(Ignore)) {
      std::cerr << "\n\twhat() == ";
      const char * what = x.what();
      if (0 != what) std::cerr << what;
    }
    std::cerr << "\n" << std::endl;
  }

  void TestHarness::ReportWarning(const std::string & msg) const {
    std::cerr << "WARNING: " << msg << std::endl;
  }

  int TestHarness::getStatus() const { return m_status; }

  void TestHarness::setStatus(int status) const { if (0 == m_status) m_status = status; }

  const std::string & TestHarness::getDataDir() const {
    if (m_data_dir.empty()) {
      m_data_dir = facilities::commonUtilities::getDataPath("tip");
      if(m_data_dir == "")
	ReportWarning("Unable to determine data path for tip package");
      else
#ifdef WIN32
        m_data_dir += "\\";
#else
        m_data_dir += "/";
#endif
    }
    return m_data_dir;
  }

}
