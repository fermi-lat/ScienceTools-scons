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
               Application: Refactor to use library to accomplish
               the most critical analysis goals: light curve,
               time-unbinned spectra (PHA1), time-binned spectra
               (PHA1 and PHA2). Critical point: files produced
               must pass fverify, and comply with the letter of
               OGIP standards for their respective file formats.

    5/23/2004  Library: 1) Add support for writing histograms to
               output (FITS) images. 2) Add support for user-provided
               bin definitions.
               Application: 1) Add capability of producing count
               maps. 2) Add capability to utilize user-provided
               bin definitions.

    5/30/2004  Library: 1) Add constant S/N and Bayesian block
               binners.
               Application: 1) Add capability of using these new
               binners.
\endverbatim

    \subsection details Library Design Details
    Class families include Binner, HistN, TipHistIO.
    The Binner hierarchy provides abstractions to manage
    bin definitions, and to determine the bin index for
    any given data value. However binners do not store any
    data. That is the job of the HistN class. HistN objects
    have one or more Binner objects, to manage binning in one
    or more dimensions. In addition, HistN objects contain an
    array to store the binned values. The TipHistIO class is
    used to facilitate binning operations in which the input
    data and/or the output binned data is stored in files
    which are accessed using the tip package.
*/
