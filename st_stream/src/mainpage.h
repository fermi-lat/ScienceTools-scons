/**
    \mainpage st_stream package

    \author James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    The st_stream package is an internal utility library for handling output
    streams for output, error, information, warning and debug information.
    Clients of this package are other packages; there are no "user serviceable"
    parts inside.

    \section OStream OStream class
    The OStream class parallels and resembles std::ostream. Objects which
    are written/shifted to an OStream object using its templated left shift
    operator are in turn left shifted to one or more std::ostream objects
    and/or to one or more additional OStream objects. OStream supports the
    notion of chattiness, so that an OStream object will only pass on
    objects to its destination streams if the OStream's message chatter
    level is less than or equal to a global maximum/cutoff chatter level.
    In addition, OStreams provide a prefix which may be prepended to
    each new line of output.

    \subsection globals Global OStream objects
    Three globally accessible OStream objects are provided in the st_stream
    namespace: sterr, stlog and stout. These parallel, and may be used
    in lieu of, the standard STL streams std::cerr, std::clog and std::cout.
    By default these streams are usually simply connected to their STL
    counterparts, but the possibilities exist that they may also be directed
    to, e.g. log files and/or GUI windows, instead of, or in addition to,
    the standard streams.

    Although they are fully featured OStream objects, these global streams
    are set up so that in effect they ignore chatter, and (at least
    initially) they have no prefixes. The intent is that these streams
    should be used just as literal replacements for their STL counterparts,
    and normally their OStream properties should not directly be modified
    by clients. Instead, the StreamFormatter class is provided to facilitate
    consistent and stylized output using chattiness and prefixes.

    \section StreamFormatter StreamFormatter class
    The StreamFormatter class wraps several OStreams with standardized
    message formatting. While clients can write directly to the global
    stream objects described above, it is hoped that they will find
    StreamFormatter objects more convenient. In addition, using StreamFormatter
    is safer, because it does not directly use the global streams, but
    only forwards output to the global streams, so any modifications made to
    chatter levels and/or prefixes will not affect the global streams.

    A StreamFormatter is created with three arguments: a string defining
    the "class name", a string defining the "method name", and an unsigned
    int giving a default chatter level. In addition, the method
    StreamFormatter::setMethod allows a client to change a "method name"
    after the StreamFormatter object has been constructed. The class name
    and method name are used, in combination with the name of the current
    executable, to define a standard prefix for output streams.

    StreamFormatter objects also provide five methods, each of which
    returns an OStream object for a specific purpose:

    \verbatim
    debug()   - Returns a stream for messages which are forwarded to sterr
                only if debugging mode is enabled. Chatter levels have
                no effect on this stream.

    error()   - Returns a stream for error messages, which are always
                forwarded to sterr. Chatter levels have no effect on this
                stream.

    info()    - Returns a stream for discretionary output, that is extra
                information which may or may not interest a user. Chatter
                levels are respected by this stream, which forwards its
                output to stlog.

    out()     - Returns a stream for output, which is always forwarded
                to stout. Chatter levels have no effect on this stream.

    warn()    - Returns a stream for diagnostic information, that is
                information about recoverable problems or other runtime
                anomalies which may or may not interest the user. Chatter
                levels are respected by this stream, which, like info(),
                forwards its output to stlog.
    \endverbatim

    For convenience, info() and warn() are also overloaded with
    a second signature taking an unsigned int chatter parameter,
    which is used to change the underlying chatter level of the
    stream in question, but does not affect the StreamFormatter
    object's default chatter level.

    \section chattiness Chattiness
    Two unsigned integers are used by an OStream object to determine
    whether a given piece of information sent to the OStream object
    will actually be forwarded to its destination(s). First, there
    is a single globally-accessible maximum chatter level, which
    should be set once when the tool begins execution, usually in
    response to user input. This number is compared to a second number
    which is a property of the individual OStream. If the OStream's
    individual chatter level is less than or equal to the globally-
    accessible maximum chatter level, objects shifted to the OStream
    object will be forwarded to its destination(s). This is intended
    to be intuitive to the end user, who will request more detailed
    output by setting the chatter level of the executable higher.

    This means that developers need to think in reverse of this logic.
    When providing messages to OStream objects, the *lower* the individual
    chatter level of the OStream object, the more likely that message
    is to be displayed. In fact, if the individual chatter level is 0,
    messages will *always* be displayed. A good mnemonic for the developer
    may be to think of messages in terms of their priority, where
    priority 0 is the top priority, followed by priority 1, 2, 3, etc.

    \section initialization Initialization
    A global static function, InitStdStreams, is provided in the st_stream
    namespace for initializing the st_stream system. This takes three
    arguments: the executable name, the maximum chatter level and a
    flag indicating whether or not to run in debug mode. This is normally
    called by the standard application main, so rarely if ever will a
    developer need to call it explicitly.

    \section examples Examples
    Please see the test code in src/test/test_st_stream.cxx, which at the
    top has several examples of how one might use st_stream.
*/
