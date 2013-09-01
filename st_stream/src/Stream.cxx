/** \file Stream.cxx
    \brief Implementation of OStream class.
    \author James Peachey, HEASARC/GSSC
*/
#include <limits>

#include "st_stream/Stream.h"

namespace st_stream {

  // Define standard streams with maximum chatter set to the highest possible value so that
  // all output sent directly to them will always be displayed.
  OStream sterr(std::numeric_limits<unsigned int>::max());
  OStream stlog(std::numeric_limits<unsigned int>::max());
  OStream stout(std::numeric_limits<unsigned int>::max());

  void OStream::initStdStreams() {
    // Connect standard streams to their natural STL counterparts.
    sterr.connect(std::cerr);
    stlog.connect(std::clog);
    stout.connect(std::cout);
  }

  OStream::OStream(unsigned int max_chat): m_std_stream_cont(), m_stream_cont(), m_prefix(), m_max_chat(max_chat),
    m_chat_level(max_chat), m_begin_new_line(true) {}

  const std::string & OStream::getPrefix() const { return m_prefix; }

  void OStream::setPrefix(const std::string prefix) { m_prefix = prefix; }

  void OStream::connect(std::ostream & dest) { m_std_stream_cont.insert(&dest); }

  void OStream::disconnect(std::ostream & dest) { m_std_stream_cont.erase(&dest); }

  void OStream::connect(OStream & dest) { m_stream_cont.insert(&dest); }

  void OStream::disconnect(OStream & dest) { m_stream_cont.erase(&dest); }

}
