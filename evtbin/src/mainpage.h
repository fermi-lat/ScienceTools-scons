/**
    \mainpage evtbin package

    \author  Yasushi Ikebe ikebe@milkyway.gsfc.nasa.gov
             James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    This package consists of a class library and an application. The
    library contains abstractions of histograms and algorithms which
    bin collections of data values into histograms. The abstractions
    in the library are generic, thus allowing them to be used for
    different and disparate applications. Also included is an interface
    to allow these histograms to be used easily in tandem with the
    tip library, to facilitate binning of data in files.

    The evtbin application is considerably more specialized. It
    operates on an input event file (and other optional input files
    as needed) to bin the event data into one of a number of standard
    data products, including light curves, spectra (PHA1 and PHA2)
    and count maps.

    \section plan Development Plan
    The first stage of development, culminating in version v0r1p0,
    was focused on providing a working application which was capable
    of the most important binning operations supported by the EventBin
    application. This version of evtbin does not have a library, but
    the application can produce light curves and simple PHA1 spectra
    with no time binning. In addition it is capable of performing
    two-dimensional binning, such as time-binned spectra and count maps,
    but it cannot actually write PHA2 or CMAP files.

    \subsection timetable Timetable
    After this stage the following plan and timetable was developed
    for completing this library and application:

\verbatim
    Week of:   Activity:
    5/10/2004  Library: 1) Develop base classes for binners and
               histograms. 2) Implement equal-interval linear
               and logarithmic binners. 3) Implement generic
               N-dimensional histogram abstractions. 4) Implement
               tip-based interface for histograms to allow filling
               histograms from files containing events, and writing
               histograms to output (FITS) tables, including both
               scalar and vector-valued columns.
               Application: No activity

    5/17/2004  Library: No activity
               Application: 5) Refactor to use library to accomplish
               the most critical analysis goals: light curve,
               time-unbinned spectra (PHA1), time-binned spectra
               (PHA1 and PHA2). Critical point: files produced
               must pass fverify, and comply with the letter of
               OGIP standards for their respective file formats.

    5/23/2004  Library: 6) Add support for writing histograms to
               output (FITS) images. 7) Add support for user-provided
               bin definitions.
               Application: 8) Add capability of producing count
               maps. 9) Add capability to utilize user-provided
               bin definitions.

    5/30/2004  Library: 10) Add constant S/N and Bayesian block
               binners.
               Application: 11) Add capability of using these new
               binners.
\endverbatim

    \subsection status Status of Development

\verbatim
    5/13/2004  Points 1, 2, 3, 4 were completed. Footnote to point
               3: a generic N-dimensional histogram class was not
               developed, but 1 and 2 D cases were, and extensions
               to 3 or more dimensions are straightforward. Point
               5 has been partly addressed. The PHA2 case does not
               work correctly, mainly because there is not yet a
               template for PHA2 files.
\endverbatim

    \section details Library Design Details
    Base classes for families include Binner and Hist. The Binner
    hierarchy provides abstractions to manage bin definitions, and
    to determine the bin index for any given data value. However
    binners do not store any data. That is the job of histograms
    in the Hist hierarchy. Hist objects use Binner in a Strategy
    pattern: a Hist object is configured with a supplied Binner
    subobject, allowing arbitrary binning methods for each dimension
    of a histogram.

    In addition, a RecordBinFiller functor exists for the purpose of
    filling a given Hist object from a table record from the tip
    package. Beyond this, tip-specific writing classes have not
    yet been added because writing output is pretty heterogeneous,
    even in very similar situations. For example, when writing a light
    curve, the interval information goes directly in the output table,
    while for spectra the intervals are described in the EBOUNDS
    extension. Similarly, the CHANNEL field is written for spectra,
    but not for light curves. For the moment, application will simply
    maintain different methods for each file output type. Such methods
    may be moved to the library at some future point.

    \section todo Open Issues
\verbatim
       1. 5/13/2004: PHA2 case doesn't work because template file is not available.
       2. 5/13/2004: Keywords in output file are wrong in many cases (e.g. DATE-OBS)
          because a means does not exist to update them from the input file.
       3. 5/13/2004: The main application, evtbin, has some redundant (parallel) code
          for different similar binning situations. This should naturally be consolidated.
\endverbatim

    \section done Resolved Issues
\verbatim
\endverbatim
*/
