#Template file for FT1 files - Aug 14, 2003 Definition

#########################
#Primary HDU template
SIMPLE   = T / file does conform to FITS standard
BITPIX   = 8 / number of bits per data pixel
NAXIS    = 0 / number of data axes
EXTEND   = T / FITS dataset may contain extensions
CHECKSUM = 0 / checksum for entire HDU
DATASUM  = 0 / checksum for data table
TELESCOP = GLAST / name of telescope generating data
INSTRUME = 'LAT' / name of instrument generating data
EQUINOX  = 2000.0 / equinox for ra and dec
RADECSYS = 'FK5' / world coord. system for this file (FK5 or FK4)
FILENAME = 'GLL_EVSUM_YYMMDD_C#_V##.FIT' / name of this file
ORIGIN   = 'LIOC' / name of organization making file
AUTHOR   = 'NAME_OF_PERSON' / name of person responsible for file generation
CREATOR  = 'EVENT_SUMMARY_MAKER_V##' / software and version creating file
VERSION  = 0.0 / integer? string? (TBD)  release version of the file
SOFTWARE = 0 / version of the processing software

######################
#LAT Event Summary HDU template
XTENSION = 'BINTABLE' / binary table extension
BITPIX   = 8 / 8-bit bytes
NAXIS    = 2 / 2-dimensional binary table
NAXIS2   = 0 / number of rows in table
CHECKSUM = 0 / checksum for entire HDU
DATASUM  = 0 / checksum for data table
TELESCOP = 'GLAST' / name of telescope generating data
INSTRUME = 'LAT' / name of instrument generating data
EQUINOX  = 2000.0 / equinox for ra and dec
RADECSYS = 'FK5' / world coord. system for this file (FK5 or FK4)
EXTNAME  = 'LARGE' / name of this binary table extension
HDUCLASS = '' / format conforms to OGIP standard
HDUCLAS1 = 'LARGE' / extension contains events
HDUCLAS2 = 'ALL' / extension contains all events detected
TSTART   = 0. / mission time of the start of the observation
TSTOP    = 0. / mission time of the end of the observation
MJDREF   = 58300. / MJD corresponding to SC clock start
TIMEUNIT = 's' / units for the time related keywords
TIMESYS  = 'TT' / type of time system that is used
TIMEREF  = 'LOCAL' / reference frame used for times
TASSIGN  = 'SATELLITE' / location where time assignment performed
CLOCKAPP = F / whether a clock drift correction has been applied
GPS_OUT  = F / whether GPS time was unavailable at any time during this interval
OBS_ID   = 0 / observation ID number
OBJECT   = 'sky' / observed object
PSR_COLS = F / whether columns for pulsar analyses are included in this file
MC_TRUTH = F / whether the Monte Carlo truth columns are included in this file
NDSKEYS  = 0 / number of data subspace keywords in header

#Column definition: required columns
TTYPE# = 'DUMMY' / dummy column
TFORM# = 'A1' / data format of field: single character string
TUNIT# = '' / physical unit of field
