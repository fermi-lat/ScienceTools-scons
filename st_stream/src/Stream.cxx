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

  void OStream::connect(OStream & dest) { if (this != &dest) m_stream_cont.insert(&dest); }

  void OStream::disconnect(OStream & dest) { m_stream_cont.erase(&dest); }

  std::ios_base::fmtflags OStream::flags() const {
    return getStreamState<std::ios_base::fmtflags, std::ios_base>(&std::ostream::flags, &OStream::flags);
  }

  std::ios_base::fmtflags OStream::flags(std::ios_base::fmtflags fmtfl) {
    return setStreamState<std::ios_base::fmtflags, std::ios_base>(&std::ostream::flags, &OStream::flags, &OStream::flags, fmtfl);
  }

  std::ios_base::fmtflags OStream::setf(std::ios_base::fmtflags fmtfl) {
    return setStreamState<std::ios_base::fmtflags, std::ios_base>(&std::ostream::setf, &OStream::setf, &OStream::flags, fmtfl);
  }

  // Note that the following method has an unusal signature and thus can't use setStreamState.
  std::ios_base::fmtflags OStream::setf(std::ios_base::fmtflags fmtfl, std::ios_base::fmtflags mask) {
    std::ios_base::fmtflags orig_flags = flags();

    // Only modify destination streams if message chatter is less than or equal to maximum user/client chatter.
    if (m_chat_level <= m_max_chat) {
      // Call setf for all std::ostream objects.
      for (StdStreamCont_t::iterator itor = m_std_stream_cont.begin(); itor != m_std_stream_cont.end(); ++itor)
        (*itor)->setf(fmtfl, mask);

      // Call setf for all OStream objects.
      for (OStreamCont_t::iterator itor = m_stream_cont.begin(); itor != m_stream_cont.end(); ++itor)
        (*itor)->setf(fmtfl, mask);
    }

    return orig_flags;
  }

  // Note that the following method has an unusal signature and thus can't use setStreamState.
  void OStream::unsetf(std::ios_base::fmtflags mask) {
    // Only modify destination streams if message chatter is less than or equal to maximum user/client chatter.
    if (m_chat_level <= m_max_chat) {
      // Call unsetf for all std::ostream objects.
      for (StdStreamCont_t::iterator itor = m_std_stream_cont.begin(); itor != m_std_stream_cont.end(); ++itor)
        (*itor)->unsetf(mask);

      // Call unsetf for all OStream objects.
      for (OStreamCont_t::iterator itor = m_stream_cont.begin(); itor != m_stream_cont.end(); ++itor)
        (*itor)->unsetf(mask);
    }
  }

  std::streamsize OStream::precision() const {
    return getStreamState<std::streamsize, std::ios_base>(&std::ostream::precision, &OStream::precision);
  }

  std::streamsize OStream::precision(std::streamsize new_precision) {
    return setStreamState<std::streamsize, std::ios_base>(&std::ostream::precision, &OStream::precision,
      &OStream::precision, new_precision);
  }

  std::streamsize OStream::width() const {
    return getStreamState<std::streamsize, std::ios_base>(&std::ostream::width, &OStream::width);
  }

  std::streamsize OStream::width(std::streamsize new_width) {
    return setStreamState<std::streamsize, std::ios_base>(&std::ostream::width, &OStream::width, &OStream::width, new_width);
  }

  char OStream::fill() const {
    return getStreamState<char, std::basic_ios<char> >(&std::ostream::fill, &OStream::fill);
  }

  char OStream::fill(char new_fill) {
    return setStreamState<char, std::basic_ios<char> >(&std::ostream::fill, &OStream::fill, &OStream::fill, new_fill);
  }

}
