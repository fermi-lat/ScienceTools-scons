/**
    \mainpage evtbin package

    \author  Yasushi Ikebe ikebe@milkyway.gsfc.nasa.gov
             James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    This package consists of a class library and an application. The
    library contains abstractions which facilitate
    binning collections of data values into histograms. The abstractions
    in the library are organized in layers ranging from generic binners
    ans histograms to representations of more specific higher level data
    types such as light curves, with file access via Tip. This is to allow
    the library to be used in maximally different and disparate contexts.

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
               (PHA2). Critical point: files produced
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

    5/20/2004  Point 5 has now been completely addressed. This version was tagged v0r2p0.
\endverbatim

    \section library_details Library Design Details
    There are currently three families used in the library.
    They are, in increasing orders of abstraction, Binner, Hist
    and DataProduct.

    \subsection binner The Binner Hierarchy
    Binner and its subclasses encapsulate the notion of binning
    in one dimension. Subclasses correspond to particular types
    of binners, e.g. LinearBinner, LogBinner, etc. These classes
    store information pertaining to the bins, but they do not themselves
    store any binned data.

    \subsection hist The Hist Hierarchy
    Hist and its subclasses encapsulate various dimensionalities of
    histograms. Hist and its subclasses contain binned data, but use
    Binner objects in a Strategy pattern to determine which bin should
    contain input values. For example, Hist2D is a
    histogram with two binning dimensions. It contains a two dimensional
    array to store the binned values, and two Binner objects which determine
    the indices in the array for a given input value.

    \subsection data_product The DataProduct Hierarchy
    DataProduct and its subclasses are higher level abstractions which
    correspond to specific histogram-based applications. In addition
    to a histogram aspect, DataProduct provides file access using
    Tip. For example, the LightCurve subclass is a front end to a
    one dimensional histogram, which can be filled from an arbitrary
    tip::Table. In addition, LightCurve has an output method which
    can write a legal OGIP-compliant light curve FITS file using Tip.

    \section app_details Application Details
    For details of the design of the application, see comments in
    evtbin.cxx.

    \section todo Open Issues
\verbatim
\endverbatim

    \section done Resolved Issues
\verbatim
       1. 5/13/2004: PHA2 case doesn't work because template file is not available.
          5/20/2004: Resolved. Template file LatBinnedTemplate is actually the correct
          template for PHA2, but the template file has 1 element in each column which
          is supposed to be a vector with multiple elements. To address this,Tip was modified
          to support changing the number of elements in a vector column. This is now
          implemented in the MultiSpec class.
       2. 5/13/2004: Keywords in output file are wrong in many cases (e.g. DATE-OBS)
          because a means does not exist to update them from the input file.
          5/20/2004: Resolved. The harvestKeywords method in DataProduct fills a container
          with keyword values from a tip::Table. These keywords are subsequently used by
          writeOutput to update keywords in the output file.
       3. 5/13/2004: The main application, evtbin, has some redundant (parallel) code
          for different similar binning situations. This should naturally be consolidated.
          5/20/2004: Resolved. A helper class hierarchy EvtBinAppBase was developed to
          allow distinct binning applications to share the common bits of their algorithms.
\endverbatim
*/
