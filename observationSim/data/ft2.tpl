# Template for FT2. August 2, 2003 definition.
# See http://glast.gsfc.nasa.gov/ssc/dev/fits_def/definitionFT2.html
#
# $Id$

SIMPLE  =                     T / File does conform to FITS standard?
BITPIX  =                     8 / Number of bits per data pixel
NAXIS   =                     0 / Number of data axes
EXTEND  =                     T / FITS dataset may contain extensions
CHECKSUM=                     0 / checksum for entire HDU
DATASUM =                     0 / checksum for data table
TELESCOP=                 GLAST / Name of telescope generating data
INSTRUME=                   LAT / Name of instrument generating data
EQUINOX =                 2000. / Equinox of celestial coord. system
RADECSYS=                   FK5 / World coord. system for this file
DATE    =                       / File creation date (YYYY-MM-DDThh:mm:ss UTC)
DATE-OBS=                       / Start date and time of the observation (UTC)
DATE-END=                       / End date and time of the observation (UTC)
FILENAME=                       / Name of this file
ORIGIN  =                       / Organization which created this file
AUTHOR  =        NAME_OF_PERSON / person responsible for file generation
CREATOR =                       / Software and version creating file
VERSION =                       / release version of the file
SOFTWARE=                       / version of the processing software
END

XTENSION= 'BINTABLE'            / binary table extension
BITPIX  =                     8 / 8-bit bytes
NAXIS   =                     2 / 2-dimensional ascii table
NAXIS1  =                       / Width of table in bytes
NAXIS2  =                       / Number of rows in table
PCOUNT  =                     0 / Size of special data area
GCOUNT  =                     1 / one data group (required keyword)
CHECKSUM=                       / checksum for entire HDU
DATASUM =                       / checksum for data table
TFIELDS =                       / number of fields in each row
TELESCOP=                       / Name of telescope generating data
INSTRUME=                   LAT / Name of instrument generating data
EQUINOX =                 2000. / Equinox of celestial coord. system
RADECSYS=                   FK5 / World coord. system for this file
DATE    =                       / File creation date (YYYY-MM-DDThh:mm:ss UTC)
DATE-OBS=                       / Start date and time of the observation (UTC)
DATE-END=                       / End date and time of the observation (UTC)
EXTNAME =                  Ext1 / Name of this binary table extension
TSTART  =                       / Mission time of the start of the observation
TSTOP   =                       / Mission time of the end of the observation
MJDREF  =               58300.0 / MJD corresponding to SC clock start
TIMEUNIT=                     s / Units of the time related keywords
TIMESYS =                    TT / Type of time system in use
TIMEREF =                 LOCAL / Reference frame used for times
TASSIGN =             SATELLITE / Location where time assignment was performed
CLOCKAPP=                     F / Whether a clock drift correction has been applied
GPS_OUT =                     F / Whether GPS time was unavailable at any time during this interval
#
# Column Definitions
#
TTYPE#  =                 START / Start time of interval (Mission Elapsed time)
TFORM#  =                     D / data format of field: 8-byte DOUBLE
TUNIT#  =                     s / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =               1.0D+10 / Maximum value

TTYPE#  =                  STOP / end time of interval (Mission Elapsed time)
TFORM#  =                     D / data format of field: 8-byte DOUBLE
TUNIT#  =                     s / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =               1.0D+10 / Maximum value
#
# Satellite position and derived quantities
TTYPE#  =           SC_POSITION / S/C position at start of interval (x,y,z inertial coord)
TFORM#  =                    3E / data format of field: 4-byte REAL
TUNIT#  =                     m / physical unit of field

TTYPE#  =               LAT_GEO / Ground point latitude
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                 -90.0 / Minimum value
TLMAX#  =                  90.0 / Maximum value

TTYPE#  =               LON_GEO / Ground point latitude
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 360.0 / Maximum value

TTYPE#  =               RAD_GEO / S/C altitude
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                    km / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =               10000.0 / Maximum value

TTYPE#  =             RA_ZENITH / RA of zenith direction at start
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 360.0 / Maximum value

TTYPE#  =            DEC_ZENITH / DEC of zenith direction at start
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                 -90.0 / Minimum value
TLMAX#  =                  90.0 / Maximum value

TTYPE#  =            B_MCILWAIN / McIlwain B parameter, magnetic field
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                 Gauss / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 100.0 / Maximum value

TTYPE#  =            L_MCILWAIN / McIlwain L parameter, distance
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =           Earth_Radii / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 100.0 / Maximum value

TTYPE#  =                IN_SAA / whether spacecraft was in SAA
TFORM#  =                     L / data format of field: logical

#
# Pointing direction and derived quantities
#
TTYPE#  =                RA_SCZ / Viewing direction at start (RA of LAT +z axis)
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 360.0 / Maximum value

TTYPE#  =               DEC_SCZ / Viewing direction at start (DEC of LAT +z axis)
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                 -90.0 / Minimum value
TLMAX#  =                  90.0 / Maximum value

TTYPE#  =                RA_SCX / Viewing direction at start (RA of LAT +x axis)
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =                 360.0 / Maximum value

TTYPE#  =               DEC_SCX / Viewing direction at start (DEC of LAT +x axis)
TFORM#  =                     E / data format of field: 4-byte REAL
TUNIT#  =                   deg / physical unit of field
TLMIN#  =                 -90.0 / Minimum value
TLMAX#  =                  90.0 / Maximum value
#
# Instrument status
#
TTYPE#  =              LAT_MODE / Operation mode of LAT
TFORM#  =                     J / data format of field: 4-byte signed INTEGER
TLMIN#  =                     0 / Minimum value
TLMAX#  =            2147483647 / Maximum value

TTYPE#  =              LIVETIME / Live time
TFORM#  =                     D / data format of field: 8-byte DOUBLE
TUNIT#  =                     s / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =               1.0D+10 / Maximum value

TTYPE#  =              DEADTIME / Dead time accumulated since start of mission
TFORM#  =                     D / data format of field: 8-byte DOUBLE
TUNIT#  =                     s / physical unit of field
TLMIN#  =                   0.0 / Minimum value
TLMAX#  =               1.0D+10 / Maximum value

