/**
    \mainpage pulsePhase package

    \author  Masaharu Hirayama hirayama@jca.umbc.edu
             James Peachey James.Peachey-1@nasa.gov

    \section synopsis Synopsis
This package contains a library and two applications,
gtpphase and gtophase.
The application gtpphase operates on an event file to compute
the spin (pulse) phase for the time of each event, and writes this
phase to the PULSE_PHASE column of the event file.
The application gtophase operates on an event file to compute
the orbital phase for the time of each event, and writes this
phase to the ORBITAL_PHASE column of the event file.

    \section parameters Parameters

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

    \subsection gtpphase_general gtpphase Parameters
\verbatim
evfile [file name]
    Name of input event file, FT1 format or equivalent.

scfile [file name]
    Name of input spacecraft data file, FT2 format or equivalent.

psrdbfile [file name]
    Name of pulsar ephemerides database file, in Fermi (formerly
    GLAST) D4 FITS format. Multiple files may be combined by listing
    them in a text file, one per line, and supplying the list file
    name preceded by an @ sign. If psrdbfile is NONE
    (case-insensitive), no ephemeris is loaded from a file.

psrname = ANY [string]
    Name of the pulsar, used to select only ephemerides valid for a
    particular pulsar.

ephstyle = DB [enumerated string (DB|FREQ|PER)]
    Method to specify how the ephemerides will be supplied. If
    ephstyle is DB, a pulsar database file in Fermi (formerly GLAST)
    D4 FITS format will be used. If ephstyle is FREQ (PER), the user
    will supply values for the frequency (period) and its derivatives
    at the time given by the epoch parameter.

ephepoch = 0. [string]
    Reference epoch of the ephemeris that will be manually specified.
    This parameter only has effect if ephstyle is FREQ or PER.

timeformat = FILE [enumerated string (FILE|MJD|FERMI|GLAST)]
    String describing the representation used for ephepoch parameter.
    If FILE is chosen, the time format specified in the input event
    file header will be used.

timesys = FILE [enumerated string (FILE|TAI|TDB|TT|UTC)]
    String describing the time system used for ephepoch parameter and
    the following ephemeris parameters: phi0, f0, f1, f2, p0, p1, and
    p2.  In other words, ephemeris computaions with those parameters
    will be performed in the time system specified by this parameter.
    If FILE is chosen, the time system specified in the input event
    file header (TIMESYS keyword) will be used.

ra [double]
    Right Ascension of point source in degrees for which to perform
    the barycentric correction.  This parameter only has effect if
    ephstyle is FREQ or PER.

dec [double]
    Declination of point source in degrees for which to perform the
    barycentric correction.  This parameter only has effect if
    ephstyle is FREQ or PER.

phi0 = 0. [double]
    Phase offset at this ephepoch for a user-supplied ephemeris.  This
    parameter only has effect if ephstyle is FREQ or PER.  Note that
    ephemeris computations using this parameter will be performed in
    the time system specified by timesys parameter.

f0 = 1. [double]
    Value of the frequency at the time given by the epoch parameter in
    the units of s^(-1).  This parameter only has effect if ephstyle
    is FREQ.  Note that ephemeris computations using this parameter
    will be performed in the time system specified by timesys
    parameter.

f1 = 0. [double]
    Value of the first time derivative of the frequency at the time
    given by the epoch parameter in the units of s^(-2).  This
    parameter only has effect if ephstyle is FREQ.  Note that
    ephemeris computations using this parameter will be performed in
    the time system specified by timesys parameter.

f2 = 0. [double]
    Value of the second time derivative of the frequency at the time
    given by the epoch parameter in the units of s^(-3).  This
    parameter only has effect if ephstyle is FREQ.  Note that
    ephemeris computations using this parameter will be performed in
    the time system specified by timesys parameter.

p0 = 1. [double]
    Value of the period at the time given by the epoch parameter in
    seconds.  This parameter only has effect if ephstyle is PER.  Note
    that ephemeris computations using this parameter will be performed
    in the time system specified by timesys parameter.

p1 = 0. [double]
    Value of the first time derivative of the period at the time given
    by the epoch parameter (dimensionless).  This parameter only has
    effect if ephstyle is PER.  Note that ephemeris computations using
    this parameter will be performed in the time system specified by
    timesys parameter.

p2 = 0. [double]
    Value of the second time derivative of the period at the time
    given by the epoch parameter in the units of s^(-1).  This
    parameter only has effect if ephstyle is PER.  Note that ephemeris
    computations using this parameter will be performed in the time
    system specified by timesys parameter.

(tcorrect = AUTO) [enumerated string (NONE|AUTO|BARY|BIN|ALL)]
    Set of arrival time corrections to apply. If tcorrect is NONE, no
    corrections will be applied. If tcorrect is BARY, only the
    barycentric correction will be applied. If tcorrect is BIN or ALL,
    an appropriate orbital ephemeris is searched for in the pulsar
    database, and if found, binary demodulation will be applied after
    the barycentric correction, and if not, an error will be
    generated. If tcorrect is AUTO, the barycentric correction will be
    applied, then the binary demodulation will be applied only when an
    orbital ephemeris is available in the pulsar database.

(solareph = JPL DE405) [enumerated string (JPL DE200|JPL DE405)]
    Solar system ephemeris for the barycentric correction.

(matchsolareph = ALL) [enumerated string (NONE|EVENT|PSRDB|ALL)]
    String that controls whether to use the name of the solar system
    ephemeris given by the solareph parameter to check the input event
    data file and to select ephemerides in the pulsar database.  If
    matchsolareph is EVENT, all the input event files are required to
    use the same solar system ephemeris for the barycentric correction
    as the one given by the solareph parameter, and an error will be
    generated if otherwise. Such an error may occur when an input
    event data file has already been applied the barycentric
    correction with a different solar system ephemeris. If
    matchsolareph is PSRDB, the string given by the solareph parameter
    is used to select the ephemerides.  If matchsolareph is ALL, both
    actions for the EVENT option and the PSRDB option will be taken.
    If matchsolareph is NONE, no selection will be performed.

(angtol = 1.e-8) [double]
    Angular tolerance in degrees in comparison of two source
    positions, one for which the barycentric correction is performed,
    and another given by RA_NOM and DEC_NOM header keyword of an event
    file to which the barycentric correction has already been applied.
    This parameter only has effect if the barycentric correction has
    been applied to an input event data file.  If the two source
    positions are separate from each other by this amount or less,
    then they will be considered to be the same position.  Otherwise
    an error will be generated.  The sign of the parameter value is
    ignored.  For example, setting angtol=-1.0 results in giving
    angular tolerance of 1 degree.

(evtable = EVENTS) [string]
    Name of the FITS table containing the event data.

(timefield = TIME) [string]
    Name of the field containing the time values for temporal
    analysis.

(sctable = SC_DATA) [string]
    Name of the FITS table containing the spacecraft data.

(pphasefield = PULSE_PHASE) [string]
    Name of the output column to contain the assigned pulse phase.

(pphaseoffset = 0.) [double]
    Global offset applied to all assigned pulse phases. This may be
    used to shift the entire pulse profile. Note that this offset will
    be applied regardless of the source of the ephemeris used, whereas
    phi0 is used only when ephstyle is FREQ or PER.

(leapsecfile = DEFAULT) [file name]
    Name of the file containing the name of the leap second table, in
    OGIP-compliant leap second table format. If leapsecfile is the
    string DEFAULT, the default leap-second file (leapsec.fits), which
    is distributed with the extFiles package, will be used.

(reportephstatus = yes) [bool]
    If reportephstatus is yes, the application will examine the input
    pulsar ephemeris database, and report findings which may affect
    the requested ephemeris computations. If reportephstatus is no, it
    will not report any ephemeris status.
\endverbatim

    \subsection gtophase_parameters gtophase Parameters

\verbatim
evfile [file name]
    Name of input event file, FT1 format or equivalent.

scfile [file name]
    Name of input spacecraft data file, FT2 format or equivalent.

psrdbfile [file name]
    Name of pulsar ephemerides database file, in Fermi (formerly
    GLAST) D4 FITS format. Multiple files may be combined by listing
    them in a text file, one per line, and supplying the list file
    name preceded by an @ sign. If psrdbfile is NONE
    (case-insensitive), no ephemeris is loaded from a file.

psrname = ANY [string]
    Name of the pulsar, used to select only ephemerides valid for a
    particular pulsar.

ra [double]
    Right Ascension of point source in degrees for which to perform
    the barycentric correction.

dec [double]
    Declination of point source in degrees for which to perform the
    barycentric correction.

(solareph = JPL DE405) [enumerated string (JPL DE200|JPL DE405)]
    Solar system ephemeris for the barycentric correction.

(matchsolareph = ALL) [enumerated string (NONE|EVENT|PSRDB|ALL)]
    String that controls whether to use the name of the solar system
    ephemeris given by the solareph parameter to check the input event
    data file and to select ephemerides in the pulsar database.  If
    matchsolareph is EVENT, all the input event files are required to
    use the same solar system ephemeris for the barycentric correction
    as the one given by the solareph parameter, and an error will be
    generated if otherwise. Such an error may occur when an input
    event data file has already been applied the barycentric
    correction with a different solar system ephemeris. If
    matchsolareph is PSRDB, the string given by the solareph parameter
    is used to select the ephemerides.  If matchsolareph is ALL, both
    actions for the EVENT option and the PSRDB option will be taken. 
    If matchsolareph is NONE, no selection will be performed.

(angtol = 1.e-8) [double]
    Angular tolerance in degrees in comparison of two source
    positions, one for which the barycentric correction is performed,
    and another given by RA_NOM and DEC_NOM header keyword of an event
    file to which the barycentric correction has already been applied.
    This parameter only has effect if the barycentric correction has
    been applied to an input event data file.  If the two source
    positions are separate from each other by this amount or less,
    then they will be considered to be the same position.  Otherwise
    an error will be generated.  The sign of the parameter value is
    ignored.  For example, setting angtol=-1.0 results in giving
    angular tolerance of 1 degree.

(evtable = EVENTS) [string]
    Name of the FITS table containing the event data.

(timefield = TIME) [string]
    Name of the field containing the time values for temporal
    analysis.

(sctable = SC_DATA) [string]
    Name of the FITS table containing the spacecraft data.

(ophasefield = ORBITAL_PHASE) [string]
    Name of the output column to contain the assigned orbital phase.

(ophaseoffset = 0.) [double]
    Global offset applied to all assigned orbital phases.

(leapsecfile = DEFAULT) [file name]
    Name of the file containing the name of the leap second table, in
    OGIP-compliant leap second table format. If leapsecfile is the
    string DEFAULT, the default leap-second file (leapsec.fits), which
    is distributed with the extFiles package, will be used.

(reportephstatus = yes) [bool]
    If reportephstatus is yes, the application will examine the input
    pulsar ephemeris database, and report findings which may affect
    the requested ephemeris computations. If reportephstatus is no, it
    will not report any ephemeris status.
\endverbatim

    \section open_issues Open Issues
\verbatim
None.
\endverbatim

    \section done Resolved Issues
\verbatim
None.
\endverbatim
*/
