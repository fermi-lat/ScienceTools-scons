/** \file test_main.cxx
    \brief "Hello world" application showing how StApp can be used as a base class for the application object.

    This also demonstrates how to use Hoops from StApp to get parameters, and how to use st_stream to
    format output.
    \author James Peachey, HEASARC/GSSC
*/
#include <string>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

/** \class TestApp1
    \brief Application singleton for test_st_app.
*/
class TestApp1 : public st_app::StApp {
  public:
    TestApp1(): m_f("TestApp1", "", 2) {}

    /** \brief Perform the demo action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      // For output streams, set name of method, which will be used in messages when tool is run in debug mode.
      m_f.setMethod("run()");

      // For parameter file access, use the AppParGroup class, which is derived from a Hoops class.
      st_app::AppParGroup & pars(getParGroup("test_st_app"));

      // Prompt for all parameters in order.
      // pars.Prompt();

      // To prompt for just the string parameter, comment out the line above and uncomment the following:.
      // pars.Prompt("string");

      // Save parameters which were just prompted for.
      pars.Save();

      // Extract the string from the parameter.
      std::string user_string = pars["string"];

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
    }

  private:
    st_stream::StreamFormatter m_f;
};

// Factory which can create an instance of the class above.
st_app::StAppFactory<TestApp1> g_factory("test_st_app");
