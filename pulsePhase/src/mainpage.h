/**
    \mainpage pulsePhase package

    \author  Masaharu Hirayama hirayama@milkyway.gsfc.nasa.gov
             James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
This package contains a library and an application.
The library's main public class is the TimingModel class,
which computes the phase of any photon arrival time,
given a particular set of ephemeral data (frequency or
period derivatives).

The application operates on an event file to compute
the phase for the time of each event, and write this
phase to the PULSE_PHASE column of the event file.

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

    \subsection general General Parameters
\verbatim
eventfile [string]
    Name of input event file, FT1 format or equivalent.

ephstyle [string]
    Specifies whether the user will supply values for the central
    frequency and its derivatives (FREQ) or the central period and
    its derivatives (PER).

epoch [double]
    The epoch, or time origin, for the ephemeris.

phi0 [double]
    The phase offset at this epoch.

(timefield = TIME) [string]
    This is the name of the field containing the time values for
    time binning. The default value is consistent with the FT1
    format.
\endverbatim


    \subsection frequency_para Frequency Ephemeris Parameters
\verbatim
f0 [double]
    The value of the frequency at the epoch. Only used if
    ephstyle is FREQ.

f1 [double]
    The value of the first time derivative of the frequency at the
    epoch. Only used if ephstyle is FREQ.

f2 [double]
    The value of the second time derivative of the frequency at the
    epoch. Only used if ephstyle is FREQ.
\endverbatim

    \subsection period_pars Period Ephemeris Parameters
\verbatim
p0 [double]
    The value of the period at the epoch. Only used if
    ephstyle is PER.

p1 [double]
    The value of the first time derivative of the period at the
    epoch. Only used if ephstyle is PER.

p2 [double]
    The value of the second time derivative of the period at the
    epoch. Only used if ephstyle is PER.
\endverbatim

    \section todo Open Issues
\verbatim
\endverbatim

    \section done Resolved Issues
\verbatim
\endverbatim
*/
