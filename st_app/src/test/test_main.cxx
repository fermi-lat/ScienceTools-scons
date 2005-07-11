/** \file test_main.cxx
    \brief "Hello world" application showing how StApp can be used as a base class for the application object.

    This also demonstrates how to use Hoops from StApp to get parameters, and how to use st_stream to
    format output.
    \author James Peachey, HEASARC/GSSC
*/
#include <stdexcept>
#include <string>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/StGui.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

const std::string s_cvs_tag("$Name$");

/** \class TestApp1
    \brief Application singleton for test_st_app.
*/
class TestApp1 : public st_app::StApp {
  public:
    TestApp1(): m_f("TestApp1", "", 2) {
      m_f.setMethod("TestApp1");

      // Confirm that initial version reporting is working correctly.
      if (getVersion() != "N/A")
        m_f.err() << "Before version id was set, getVersion returned \"" << getVersion() << "\" not \"N/A\"." << std::endl;

      // Set name and version of executable.
      setName("test_st_app");
      setVersion(s_cvs_tag);

      // For parameter file access, use the AppParGroup class, which is derived from a Hoops class.
      st_app::AppParGroup & pars(getParGroup());

      pars.setSwitch("switch");
      pars.setSwitch("usedeltae");

      pars.setCase("switch", "EnERGY", "emin");
      pars.setCase("switch", "energy", "emax");
      pars.setCase("switch", "ENergy", "usedeltae");
      pars.setCase("usedeltae", "true", "deltae");
      pars.setCase("switch", "ENergy", "deltae");
      pars.setCase("switch", "Time", "tstart");
      pars.setCase("switch", "TIME", "tstop");
      pars.setCase("switch", "timE", "deltat");
    }

    /** \brief Perform the demo action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      bool failed = false;

      // For output streams, set name of method, which will be used in messages when tool is run in debug mode.
      m_f.setMethod("run()");

      // Test resetting version to a blank.
      std::string correct_version = "N/A";
      setVersion(" \t");
      if (correct_version != getVersion()) {
        m_f.err() << "After setVersion(\" \\t\"), version is \"" << getVersion() << "\", not \"" <<
          correct_version << "\", as expected." << std::endl;
        failed = true;
      }

      // Test resetting version to a blank cvs tagged version number (HEAD).
      correct_version = "HEAD";
      setVersion("$Name$");
      if (correct_version != getVersion()) {
        m_f.err() << "After setVersion(\"$Name$\"), version is \"" << getVersion() << "\", not \"" <<
          correct_version << "\", as expected." << std::endl;
        failed = true;
      }

      // Test resetting version to a real cvs tagged version number.
      correct_version = "v0";
      setVersion("$Name$");
      if (correct_version != getVersion()) {
        m_f.err() << "After setVersion(\"$Name$\"), version is \"" << getVersion() << "\", not \"" <<
          correct_version << "\", as expected." << std::endl;
        failed = true;
      }

      // Test resetting version to something custom.
      correct_version = "custom";
      setVersion("custom");
      if (correct_version != getVersion()) {
        m_f.err() << "After setVersion(\"custom\"), version is \"" << getVersion() << "\", not \"" <<
          correct_version << "\", as expected." << std::endl;
        failed = true;
      }

      // Finally, reset version to what it should normally be.
      std::string correct_tag = (s_cvs_tag == "$Name$") ? "$Name$" : s_cvs_tag;
      setVersion(s_cvs_tag);
      std::string tag = "$Name$";
      if (correct_tag != tag) {
        m_f.err() << "Version tag \"" << tag << "\" is not \"" << correct_tag << "\", as expected." << std::endl;
        failed = true;
      }

      // For parameter file access, use the AppParGroup class, which is derived from a Hoops class.
      st_app::AppParGroup & pars(getParGroup());

      // File parameters must be set to a file which exists.
      pars["infile"] = ".";
      pars["outfile"] = ".";

      // Do not commit version with prompting, as it will break automated tests.
      // Prompt for all parameters in order.
      // pars.Prompt();

      // Do not commit version with prompting, as it will break automated tests.
      // To prompt for just the string parameter, comment out the line above and uncomment the following:
      // pars.Prompt("string");

      // Save parameters which were just prompted for.
      pars.Save();

      // Reset stream's debug mode to match global mode.
      m_f.setDebugMode(st_app::IStAppFactory::instance().getDebugMode());

      // Extract the string from the parameter.
      std::string user_string = pars["string"];
      std::string in_file = pars["infile"];

      // Next Demonstrate how st_stream formats output.
      // The "info" stream is for optional output.
      m_f.info(0) << "This info should always be displayed." << std::endl <<
        st_stream::Chat(1) << "This info should be displayed only if chatter >= 1." << std::endl;
      m_f.info() << "This info should be displayed only if chatter >= 2." << std::endl;
      m_f.info(3) << "This info should be displayed only if chatter >= 3." << std::endl;

      // The "warn" stream is for recovered errors.
      m_f.warn(3) << "If something a bit odd happened, write it to the warn stream." << std::endl;

      // The "err" stream is for serious/unrecoverable errors.
      m_f.err() << "If something really bad happened, write it to the err stream." << std::endl;

      // The "debug" stream is for debugging messages, so this only shows up if debug=true.
      m_f.debug() << "This is a debugging statement." << std::endl;

      // The "out" stream is for tool output.
      m_f.out() << "The string the user entered was:" << std::endl;
      m_f.out() << user_string << std::endl;
      m_f.out() << "The infile the user entered was:" << std::endl;
      m_f.out() << in_file << std::endl;

      if (failed) throw std::runtime_error("Test failed");
    }

    virtual void runGui() {
      st_app::StAppGui gui(st_graph::Engine::instance(), getParGroup(), this);
      gui.run();
    }

  private:
    st_stream::StreamFormatter m_f;
};

// Factory which can create an instance of the class above.
st_app::StAppFactory<TestApp1> g_factory("test_st_app");
