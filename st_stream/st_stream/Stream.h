/** \file Stream.h
    \brief Declaration of OStream class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_stream_Stream_h
#define st_stream_Stream_h

#include <iostream>
#include <set>
#include <string>

namespace st_stream {

  /** \class OStream
      \brief Output stream class which connects its output to one or more std::ostreams, and/or to
             one or more other OStreams.
  */
  class OStream {
    public:
      /** \brief Type of container of std::ostreams. */
      typedef std::set<std::ostream *> StdStreamCont_t;

      /** \brief Type of container of OStreams. */
      typedef std::set<OStream *> OStreamCont_t;

      /** \brief Perform initializations of globally accessible streams sterr, stlog and stout.
      */
      static void initStdStreams();

      /** \brief Create an OStream with the given client maximum chatter.
          \param max_chat The maximum chatter level of output selected by the user/client.
      */
      OStream(unsigned int max_chat);

      /** \brief Shift the given object to the destination stream(s), but only if the current
                 message chatter level is less than or equal to the maximum chatter level.
          \param t The object to shift.
      */
      template <typename T>
      OStream & operator <<(const T & t);

      /** \brief Pass the given stream modifier to the destination stream(s), but only if the current
                 message chatter level is less than or equal to the maximum chatter level.
          \param func The stream modifier.
      */
      OStream & operator <<(std::ostream & (*func)(std::ostream &));

      /** \brief Change the current chatter level for messages written to the stream. This has no
                 effect on the maximum chatter level currently selected by the user/client.

                 If the message chatter level is greater than the maximum client chatter level, future
                 output will not be sent to any of this stream's destinations.
          \param chat_level The new chatter level for messages to be written to the output stream.
      */
      OStream & setChatLevel(unsigned int chat_level);

      /** \brief Return the prefix string which precedes each new line of output on this stream.
      */
      const std::string & getPrefix() const;

      /** \brief Set the prefix string which precedes each new line of output on this stream.
          \param prefix The new prefix to use.
      */
      void setPrefix(const std::string prefix);

      /** \brief Connect a destination stream to the output of this stream. Output from this stream will
                 be forwarded to the destination.
          \param dest The destination stream being connected.
      */
      void connect(std::ostream & dest);

      /** \brief Disconnect a destination stream to the output of this stream. Output from this stream will
                 no longer be forwarded to the destination.
          \param dest The destination stream being disconnected.
      */
      void disconnect(std::ostream & dest);

      /** \brief Connect a destination stream to the output of this stream. Output from this stream will
                 be forwarded to the destination.
          \param dest The destination stream being connected.
      */
      void connect(OStream & dest);

      /** \brief Disconnect a destination stream to the output of this stream. Output from this stream will
                 no longer be forwarded to the destination.
          \param dest The destination stream being disconnected.
      */
      void disconnect(OStream & dest);

    private:
      StdStreamCont_t m_std_stream_cont;
      OStreamCont_t m_stream_cont;
      std::string m_prefix;
      unsigned int m_max_chat;
      unsigned int m_chat_level;
      bool m_begin_new_line;
  };

  /** \class Chat
      \brief Functor utility to change the chatter level of a stream at any point in a line of shift operators.
  */
  class Chat {
    public:
      /** \brief Create a Chat object with the given message chatter level.
          \param chat_level The message chatter level.
      */
      Chat(unsigned int chat_level): m_chat_level(chat_level) {}

      /** \brief Functor operator which modifies the message chatter level of the target stream. The maximum
                 chatter level of the stream is unaffected.
          \param os The output stream whose chatter level will be set.
      */
      OStream & operator()(OStream & os) const { return os.setChatLevel(m_chat_level); }

    private:
      unsigned int m_chat_level;
  };

  /** \brief Shift operator used to shift a Chat object to an OStream object.
      \param os The OStream object acted upon by the Chat object.
      \param chat The Chat object which modifies the OStream object.
  */
  inline OStream & operator <<(OStream & os, const Chat & chat) { return chat(os); }

  template <typename T>
  inline OStream & OStream::operator <<(const T & t) {
    // Only send object to destination(s) if message chatter is less than maximum user/client chatter.
    if (m_chat_level <= m_max_chat) {
      // Iterate over std::ostreams, shifting object to each in turn.
      for (StdStreamCont_t::iterator itor = m_std_stream_cont.begin(); itor != m_std_stream_cont.end(); ++itor) {
        // Prepend prefix if this is the first object being shifted at the beginning of a new line.
        if (m_begin_new_line && !m_prefix.empty()) *(*itor) << m_prefix;
        // Shift the object itself.
        *(*itor) << t;
      }
      // Iterate over OStreams, shifting object to each in turn.
      for (OStreamCont_t::iterator itor = m_stream_cont.begin(); itor != m_stream_cont.end(); ++itor) {
        // Prepend prefix if this is the first object being shifted at the beginning of a new line.
        if (m_begin_new_line && !m_prefix.empty()) *(*itor) << m_prefix;
        // Shift the object itself.
        *(*itor) << t;
      }
      // Something was shifted, so no longer the beginning of a new line.
      m_begin_new_line = false;
    }
    return *this;
  }

  inline OStream & OStream::operator <<(std::ostream & (*func)(std::ostream &)) {
    // Only send modifier to destination(s) if message chatter is less than maximum user/client chatter.
    if (m_chat_level <= m_max_chat) {
      // Iterate over std::ostreams, shifting object to each in turn.
      for (StdStreamCont_t::iterator itor = m_std_stream_cont.begin(); itor != m_std_stream_cont.end(); ++itor) {
        *(*itor) << func;
      }
      // Iterate over OStreams, shifting object to each in turn.
      for (OStreamCont_t::iterator itor = m_stream_cont.begin(); itor != m_stream_cont.end(); ++itor) {
        *(*itor) << func;
      }
      // If this modifier is endl, set flag indicating position at the beginning of a new line.
      if (static_cast<std::ostream &(*)(std::ostream&)>(std::endl<std::ostream::char_type, std::ostream::traits_type>) == func)
        m_begin_new_line = true;
    }
    return *this;
  }

  inline OStream& OStream::setChatLevel(unsigned int chat_level) {
    m_chat_level = chat_level;
    return *this;
  }

  /** \brief Error stream, parallel to std::cerr. This stream has the highest possible maximum chatter, so all
             output sent directly to it will be displayed. This stream has no prefix.
  */
  extern OStream sterr;

  /** \brief Log stream, parallel to std::clog. This stream has the highest possible maximum chatter, so all
             output sent directly to it will be displayed. This stream has no prefix.
  */
  extern OStream stlog;

  /** \brief Output stream, parallel to std::cout. This stream has the highest possible maximum chatter, so all
             output sent directly to it will be displayed. This stream has no prefix.
  */
  extern OStream stout;

}

#endif
