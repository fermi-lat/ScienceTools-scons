/** \file st_stream.cxx
    \brief Implementation of globally accessible stream setup and info methods.
    \author James Peachey, HEASARC/GSSC
*/
#include <limits>
#include "st_stream/st_stream.h"

namespace st_stream {

  // Global debug mode flag.
  static bool s_debug_mode = false;

  // Name of the current executable.
  static std::string s_exec_name;

  // Global chatter maximum.
  static unsigned int s_global_max_chat = std::numeric_limits<unsigned int>::max();

  void InitStdStreams(const std::string & exec_name, unsigned int max_chat, bool debug_mode) {
    // Perform initialization only once.
    static bool s_init_done = false;

    if (!s_init_done) {
      // Initialize sterr, stlog and stout.
      OStream::initStdStreams();

      // Set global parameters affecting stream output.
      s_debug_mode = debug_mode;
      s_exec_name = exec_name;
      s_global_max_chat = max_chat;
      s_init_done = true;
    }
  }

  bool GetDebugMode() {
    return s_debug_mode;
  }

  const std::string & GetExecName() {
    return s_exec_name;
  }

  unsigned int GetMaximumChatter() {
    return s_global_max_chat;
  }

  void SetDebugMode(bool debug_mode) {
    s_debug_mode = debug_mode;
  }

  void SetExecName(const std::string & exec_name) {
    s_exec_name = exec_name;
  }

  void SetMaximumChatter(unsigned int max_chat) {
    s_global_max_chat = max_chat;
  }

}
