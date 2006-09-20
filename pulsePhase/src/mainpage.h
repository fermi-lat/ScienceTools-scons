/**
    \mainpage pulsePhase package

    \author  Masaharu Hirayama hirayama@jca.umbc.edu
             James Peachey peachey@milkyway.gsfc.nasa.gov

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

    \subsection gtpphase_general gtpphase General Parameters

\verbatim
evfile [file name]
    Name of input event file, FT1 format or equivalent.

psrname = ANY [string]
    The name of the pulsar, used to select only ephemerides
    valid for a particular pulsar.

ephstyle = DB [string]
    Specifies how the ephemerides will be supplied. If ephstyle
    is DB, a pulsar database file (GLAST D4 FITS format) will
    be used. If ephstyle is FREQ (PER), the user will supply values
    for the frequency (period) and its derivatives at the time
    given by the ephepoch parameter.

ephepoch = 0. [string]
    The epoch, or time origin, for a user-supplied ephemeris. This
    parameter only has effect if ephstyle is FREQ or PER.

timeformat = FILE [string]
    String describing the representation used for the ephepoch.
    Valid choices are FILE, MJD and GLAST (MET). If FILE is chosen,
    the time format specified in the input event file header will be
    used.

timesys = FILE [string]
    String describing the time system used for the ephepoch.
    Valid choices are FILE, TAI, TDB, TT and UTC. If FILE is chosen,
    the time system specified in the input event file header (TIMESYS
    keyword) will be used.

phi0 = 0. [double]
    The phase offset at this ephepoch for a user-supplied ephemeris.
    This parameter only has effect if ephstyle is FREQ or PER.

(psrdbfile = DEFAULT) [file name]
    Name of pulsar ephemerides database file, in GLAST D4
    FITS format. If psrdbfile is DEFAULT, the canonical pulsar
    database file (master_pulsardb.fits), which is distributed
    with the pulsar tools, will be used.

(demodbin = AUTO) [string]
    A three-way switch. If demodbin is AUTO, binary demodulation
    will be applied only to pulsars for which orbital parameters
    exist in the pulsar database. If demodbin is YES, the computation
    is the same as for AUTO, but orbital parameters are required
    to be present and an error will be thrown if none are found.
    If demodbin is NO, binary demodulation will not be performed
    regardless of whether orbital ephemerides exist in the database.

(evtable = EVENTS) [string]
    The name of the file containing the event data.

(timefield = TIME) [string]
    This is the name of the field containing the time values
    for time binning. The default value is consistent with
    the GLAST FT1 event file format.

(pphasefield = PULSE_PHASE) [string]
    The name of the output column to contain the assigned pulse phase.

(pphaseoffset = 0.) [double]
    Global offset applied to all assigned pulse phases. This may be
    used to shift the entire pulse profile. Note that this offset will
    be applied regardless of the source of the ephemeris used, whereas
    phi0 is used only when ephstyle is FREQ or PER.

(leapsecfile = DEFAULT) [file name]
    The file containing the name of the leap second table, in
    OGIP-compliant leap second table format. If leapsecfile is
    the string DEFAULT, the default leapsec file (leapsec.fits),
    which is distributed with the extFiles package, will be used.

\endverbatim

    \subsection frequency_pars Frequency Ephemeris Parameters

\verbatim
f0 = 1. [double]
    The value of the frequency at the ephepoch. Only used if
    ephstyle is FREQ.

f1 = 0. [double]
    The value of the first time derivative of the frequency at the
    ephepoch. Only used if ephstyle is FREQ.

f2 = 0. [double]
    The value of the second time derivative of the frequency at the
    ephepoch. Only used if ephstyle is FREQ.
\endverbatim

    \subsection period_pars Period Ephemeris Parameters
\verbatim
p0 = 1. [double]
    The value of the period at the ephepoch. Only used if
    ephstyle is PER.

p1 = 0. [double]
    The value of the first time derivative of the period at the
    ephepoch. Only used if ephstyle is PER.

p2 = 0. [double]
    The value of the second time derivative of the period at the
    ephepoch. Only used if ephstyle is PER.
\endverbatim

    \subsection gtophase_parameters gtophase Parameters

\verbatim
evfile [file name]
    Name of input event file, FT1 format or equivalent.

psrname = ANY [string]
    The name of the pulsar, used to select only ephemerides
    valid for a particular pulsar.

(psrdbfile = DEFAULT) [file name]
    Name of pulsar ephemerides database file, in GLAST D4
    FITS format. If psrdbfile is DEFAULT, the canonical pulsar
    database file (master_pulsardb.fits), which is distributed
    with the pulsar tools, will be used.

(evtable = EVENTS) [string]
    The name of the file containing the event data.

(timefield = TIME) [string]
    This is the name of the field containing the time values
    for time binning. The default value is consistent with
    the GLAST FT1 event file format.

(ophasefield = ORBITAL_PHASE) [string]
    The name of the output column to contain the assigned orbital phase.

(ophaseoffset = 0.) [double]
    Global offset applied to all assigned orbital phases.

\endverbatim

    \section todo Open Issues
\verbatim
None.
\endverbatim

    \section done Resolved Issues
\verbatim
None.
\endverbatim
*/
