/**
    \mainpage evtbin package

    \author  Yasushi Ikebe ikebe@milkyway.gsfc.nasa.gov
             James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    This package consists of a class library and two applications. The
    library contains abstractions which facilitate
    binning collections of data values into histograms. The abstractions
    in the library are organized in layers ranging from generic binners
    ans histograms to representations of more specific higher level data
    types such as light curves, with file access via Tip. This is to allow
    the library to be used in maximally different and disparate contexts.

    The <a href=#evtbin_parameters> evtbin </a> application is considerably
    more specialized. It operates on an input file with time and/or energy
    information (and other optional input files
    as needed) to bin the event data into one of a number of standard
    data products, including light curves, spectra (PHA1 and PHA2)
    and count maps.

    The <a href=#gtbindef_parameters> gtbindef </a> application is a utility
    to assist in creating valid time and energy bin definition files. Energy bin
    definitions are in a file format which is identical to EBOUNDS extensions,
    except that the name of the extension is ENERGYBINS. Similarly, time bin
    definition files are in a format which is identical to GTI extensions,
    except that the name of the extension is TIMEBINS. The gtbindef application
    allows a user to create a file containing either an ENERGYBINS or TIMEBINS
    extension from a simple ASCII input file giving the start and stop value
    of each bin.

    \section parameters Application Parameters

    \subsection key Key To Parameter Descriptions
\verbatim
Automatic parameters:
par_name [ = value ] type

Hidden parameters:
(par_name = value ) type

Where "par_name" is the name of the parameter, "value" is the
default value, and "type" is the type of the parameter. The
type is enclosed in square brackets.

Examples:
infile [file]
    Describes an automatic (queried) file-type parameter with
    no default value.

(plot = yes) [bool]
    Describes a hidden bool-type parameter named plot, whose
    default value is yes (true).
\endverbatim

    <a name="evtbin_parameters"></a>
    \section evtbin_parameters Evtbin Application
    The evtbin application is the main application used to perform various kinds
    of binning.

    \subsection general General Parameters
\verbatim
algorithm [string]
    Indicates which specific binning to perform by indicating the
    type of output file produced. Legal values are CMAP (count map
    binned in sky X-Y projected coordinates), LC (light curve binned
    in time only), PHA1 (spectrum binned in energy), and PHA2
    (spectra binned in energy for a series of time ranges/bins).

eventfile [file]
    Name of input event file, FT1 format or equivalent.

outfile [file]
    Name of output file, whose contents are determined by the
    algorithm parameter.

scfile [file]
    Name of input spacecraft data file, FT2 format or equivalent.
\endverbatim


    \subsection timebins Time Binning Parameters
\verbatim
timebinalg = LIN [string]
    Indicates how the time bins will be specified. Legal values
    are FILE (bins will be read from a bin definition file), LIN
    (linearly uniform bins), and LOG (logarithmically uniform bins).
    This is only used if time binning is required by the output
    type selected by the algorithm parameter.

(timefield = TIME) [string]
    This is the name of the field containing the time values for
    time binning. The default value is consistent with the FT1
    format.

tstart [double]
    The start time of the first interval for linearly or
    logarithmically uniform bins. Only used if timebinalg
    is LIN or LOG.

tstop [double]
    The stop time of the last interval for linearly or
    logarithmically uniform bins. Only used if timebinalg
    is LIN or LOG.

tnumbins [integer]
    The number of bins for logarithmically uniform bins. Only
    used if timebinalg is LOG.

deltatime [double]
    The width of linearly uniform bins. Only used if timebinalg
    is LIN.

timebinfile [file]
    The name of the time bin definition file. Only used if
    timebinalg is FILE.
\endverbatim

    \subsection image Image Parameters
\verbatim
numxpix [integer]
    The number of pixels in the horizontal dimension in output
    image (maps only).

numypix [integer]
    The number of pixels in the vertical dimension in output
    image (maps only).

pixscale [double]
    The number of degrees per pixel at the center of the image
    (maps only).

xref [double]
    The horizontal position of the center of the image, either RA
    in celestial coordinates or l in galactic coordinates (maps only).

yref [double]
    The vertical position of the center of the image, either DEC
    in celestial coordinates or b in galactic coordinates (maps only).

(rafield = RA) [string]
    The field in the input file which contains the RA (maps only).
    The default value is that used in FT1 files.

(decfield = RA) [string]
    The field in the input file which contains the DEC (maps only).
    The default value is that used in FT1 files.

axisrot = 0. [double]
    The rotation angle desired for the image (maps only).

proj = AIT [string]
    The projection method. See the astro package for
    documentation of supported projections.

uselb = yes [bool]
    Indicates whether the xref and yref fields specify (RA, DEC)
    or (l, b).

\endverbatim

    \subsection energybins Energy Binning Parameters
\verbatim
energybinalg = LOG [string]
    Indicates how the energy bins will be specified. Legal values
    are FILE (bins will be read from a bin definition file), LIN
    (linearly uniform bins), and LOG (logarithmically uniform bins).
    This is only used if energy binning is required by the output
    type selected by the algorithm parameter.

(energyfield = ENERGY) [string]
    This is the name of the field containing the energy values for
    energy binning. The default value is consistent with the FT1
    format.

emin [double]
    The lowest energy of the first interval for linearly or
    logarithmically uniform bins. Only used if energybinalg
    is LIN or LOG.

emax [double]
    The highest energy of the last interval for linearly or
    logarithmically uniform bins. Only used if energybinalg
    is LIN or LOG.

enumbins [integer]
    The number of bins for logarithmically uniform bins. Only
    used if energybinalg is LOG.

deltaenergy [double]
    The width of linearly uniform bins. Only used if energybinalg
    is LIN.

energybinfile [file]
    The name of the energy bin definition file. Only used if
    energybinalg is FILE.
\endverbatim

    <a name="gtbindef_parameters"></a>
    \section gtbindef_parameters Gtbindef Application
    The gtbindef application is a utility to allow users to
    create time and energy bin definition files.

\verbatim
bintype [string]
    Specifies whether to generate time bins or energy bins

binfile [file]
    Input file containing flat ASCII definitions of the start and stop
    of each bin.

outfile [file]
    Name of the output bin definition file.

energyunits [string]
    Energy units used in the input ASCII text file. The output table
    is in the EBOUNDS format, and always has energy in keV. This
    parameter is only used if energy bins are being constructed.
\endverbatim

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

    5/24/2004  Library: 6) Add support for writing histograms to
               output (FITS) images. 7) Add support for user-provided
               bin definitions.
               Application: 8) Add capability of producing count
               maps. 9) Add capability to utilize user-provided
               bin definitions.

    5/31/2004  Library: 10) Add constant S/N and Bayesian block
               binners.
               Application: 11) Add capability of using these new
               binners.

    6/22/2004  12) For DC2: Add ability to write a bin definition file.
               Add a generic template for writing bin definition files
               for energy and time bins (generic format preferred).
               Add a boolean parameter asking user whether bin def file
               should be written, then what name to give it.
               13) For DC2: GTI extension from input file should be written in
               all output files. If time binning was performed, the GTIs
               need to be the intersection of the original GTIs with the
               tstart/tstop given by the binning. Open issue: Does U2
               tool exclude events which are in gaps when U2 creates the GTI?
               If so, then evtbin need not worry about this case. Otherwise,
               evtbin needs to prefilter events based on GTI before binning.
               Open issue: in PHA2 case need to know which GTI extension applies
               to which spectrum. This is not standardized, but see what other
               missions do. Drop PHA2 support until after DC2?
               14) Post-DC2: evtbin needs to compute livetime from
               GTI and FT2 data. FT2 gives every 30 seconds the fraction of
               live time, so evtbin does sum (GTI/30 * live fraction) over all GTIs.
               etc. etc. Tedious!
               15) For DC2: Ebounds extension needs to be filled for pha1 and pha2.
               16) For DC2: Count map needs WCS (CR*) keywords.
               17) For DC2: Keywords: DETCHANS keyword == # channels in spectrum (pha1 & pha2).
               TSTART/TSTOP taken from left/right edge of first/last GTI.
               EXPOSURE keyword is == livetime from point 14. ONTIME is sum of all GTI.
               For DC2, write EXPOSURE == ONTIME, but eventually EXPOSURE needs to be
               computed properly. Copy TIMESYS, MJDREF, EQUINOX and RADECSYS from event file! Set
               FILENAME keyword. For now, write TIMEZERO == 0.
               18) Post DC2: Spatial/energy binning, multiple spectra for a single GTI,
               using Ciao library?
               19) For DC2: ALL SPECTRA in keV, no matter what!
               20) For DC2?? TIMEZERO hidden parameter, subtract offset from times, and
               write TIMEZERO accordingly.
               21) GTI is always written for all output file types.

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

    6/22/2004  Points 6, 7, 8, 9 were addressed some time ago. Points 17 and 15 were completed
               today.

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
    store any binned data. Binner-derived classes have a virtual method
    called computeIndex, which determines the index (bin number) for
    a given value. In this way, the act of binning the data, which
    varies from binner to binner, is decoupled from the act of storing
    the data, which does not depend on the binning method.

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
       4.  5/24/2004: Currently there are only two binning methods: LinearBinner and LogBinner. New
           subclasses of Binner should be added to support User-defined binners, Constant Signal-to-Noise
           binners and Bayesian Block binners.
       6.  5/24/2004: TIMEDEL column is always written even for constant-size time bins.
       7.  5/24/2004: SingleSpec and MultiSpec write only channel and counts. There are plenty of other
           fields which need to be populated.
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
       5.  8/16/2004: Resolved. All binners now write GTIs. (Original issue was: The LightCurve class currently
          does not write GTIs.
\endverbatim
*/
