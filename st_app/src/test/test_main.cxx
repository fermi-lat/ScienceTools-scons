/** \file test_main.cxx
    \brief "Hello world" application showing how StApp can be used as a base class for the application object.
*/
#include <stdexcept>

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

    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      m_f.setMethod("run()");
      m_f.info(0) << "This should always be displayed." << std::endl <<
        st_stream::Chat(1) << "This should be displayed if chatter >= 1." << std::endl;
      m_f.info() << "This should be displayed if chatter >= 2." << std::endl;
      m_f.info(3) << "This should be displayed if chatter >= 3." << std::endl;
      m_f.out() << "Hello world" << std::endl;
    }

  private:
    st_stream::StreamFormatter m_f;
};

// Factory which can create an instance of the class above.
st_app::StAppFactory<TestApp1> g_factory("test_st_app");
