# Definition of LAT Event Summary File (FT1)
# Definition as of August 2nd, 2004
SIMPLE      = T                              / file does conform to FITS standard
BITPIX      = 8                              / number of bits per data pixel
NAXIS       = 0                              / number of data axes
EXTEND      = T                              / FITS dataset may contain extensions
CHECKSUM    =                                / checksum for entire HDU
DATASUM     =                                / checksum for data table
TELESCOP    = 'GLAST'                        / name of telescope generating data
INSTRUME    = 'LAT'                          / name of instrument generating data
EQUINOX     = 2000.0                         / equinox for ra and dec
RADECSYS    = 'FK5'                          / world coord. system for this file (FK5 or FK4)
DATE        =                                / file creation date (YYYY-MM-DDThh:mm:ss UT)
DATE-OBS    =                                / start date and time of the observation (UTC)
DATE-END    =                                / end date and time of the observation (UTC)
FILENAME    =                                / name of this file
ORIGIN      = ''                             / name of organization making file
AUTHOR      = ''                             / name of person responsible for file generation
CREATOR     = ''                             / software and version creating file
VERSION     =                                / release version of the file
SOFTWARE    =                                / version of the processing software
END

XTENSION    = 'BINTABLE'                                / binary table extension
BITPIX      = 8                                         / 8-bit bytes
NAXIS       = 2                                         / 2-dimensional binary table
NAXIS1      =                                           / width of table in bytes
NAXIS2      =                                           / number of rows in table
PCOUNT      =                                           / size of special data area
GCOUNT      = 1                                         / one data group (required keyword)
TFIELDS     =                                           / number of fields in each row
CHECKSUM    =                                           / checksum for entire HDU
DATASUM     =                                           / checksum for data table
TELESCOP    = 'GLAST'                                   / name of telescope generating data
INSTRUME    = 'LAT'                                     / name of instrument generating data
EQUINOX     = 2000.0                                    / equinox for ra and dec
RADECSYS    = 'FK5'                                     / world coord. system for this file (FK5 or FK4)
DATE        =                                           / file creation date (YYYY-MM-DDThh:mm:ss UT)
DATE-OBS    =                                           / start date and time of the observation (UTC)
DATE-END    =                                           / end date and time of the observation (UTC)
EXTNAME     = 'EVENTS'                                  / name of this binary table extension
HDUCLASS    = 'OGIP'                                    / format conforms to OGIP standard
HDUCLAS1    = 'EVENTS'                                  / extension contains events
HDUCLAS2    = 'ALL'                                     / extension contains all events detected
TSTART      =                                           / mission time of the start of the observation
TSTOP       =                                           / mission time of the end of the observation
MJDREF      = 54101.0                                   / MJD corresponding to SC clock start
TIMEUNIT    = 's'                                       / units for the time related keywords
TIMESYS     = 'TT'                                      / type of time system that is used
TIMEREF     = 'LOCAL'                                   / reference frame used for times
TASSIGN     = 'SATELLITE'                               / location where time assignment performed
CLOCKAPP    =                                           / whether a clock drift correction has been applied
GPS_OUT     =                                           / whether GPS time was unavailable at any time during this interval
OBS_ID      =                                           / observation ID number
OBJECT      = ''                                        / observed object
MC_TRUTH    =                                           / whether the Monte Carlo truth columns are included in this file
TTYPE1      = 'ENERGY'                                  / energy of event
TFORM1      = 'E'                                       / data format of field: 4-byte REAL
TUNIT1      = 'MeV'                                     / physical unit of field
TLMIN1      = 0.0                                       / minimum value
TLMAX1      = 1.0e+7                                    / maximum value
TTYPE2      = 'RA'                                      / right ascension (J2000) of event
TFORM2      = 'E'                                       / data format of field: 4-byte REAL
TUNIT2      = 'deg'                                     / physical unit of field
TLMIN2      = 0.0                                       / minimum value
TLMAX2      = 360.0                                     / maximum value
TTYPE3      = 'DEC'                                     / declination (J2000) of event
TFORM3      = 'E'                                       / data format of field: 4-byte REAL
TUNIT3      = 'deg'                                     / physical unit of field
TLMIN3      = -90.0                                     / minimum value
TLMAX3      = 90.0                                      / maximum value
TTYPE4      = 'THETA'                                   / inclination angle of event in instrument coordinates
TFORM4      = 'E'                                       / data format of field: 4-byte REAL
TUNIT4      = 'deg'                                     / physical unit of field
TLMIN4      = 0.0                                       / minimum value
TLMAX4      = 180.0                                     / maximum value
TTYPE5      = 'PHI'                                     / azimuthal angle of event in instrument coordinates
TFORM5      = 'E'                                       / data format of field: 4-byte REAL
TUNIT5      = 'deg'                                     / physical unit of field
TLMIN5      = 0.0                                       / minimum value
TLMAX5      = 360.0                                     / maximum value
TTYPE6      = 'ZENITH_ANGLE'                            / zenith angle of event
TFORM6      = 'E'                                       / data format of field: 4-byte REAL
TUNIT6      = 'deg'                                     / physical unit of field
TLMIN6      = 0.0                                       / minimum value
TLMAX6      = 180.0                                     / maximum value
TTYPE7      = 'EARTH_AZIMUTH_ANGLE'                     / Earth azimuth (from north to east) of event
TFORM7      = 'E'                                       / data format of field: 4-byte REAL
TUNIT7      = 'deg'                                     / physical unit of field
TLMIN7      = 0.0                                       / minimum value
TLMAX7      = 360.0                                     / maximum value
TTYPE8      = 'TIME'                                    / Mission Elapsed Time
TFORM8      = 'D'                                       / data format of field: 8-byte DOUBLE
TUNIT8      = 's'                                       / physical unit of field
TLMIN8      = 0.0                                       / minimum value
TLMAX8      = 1.0D+10                                   / maximum value
TTYPE9      = 'EVENT_ID'                                / ID number of original event
TFORM9      = 'J'                                       / data format of field: 4-byte signed INTEGER
TLMIN9      = 0                                         / minimum value
TLMAX9      = 2147483647                                / maximum value
TTYPE10     = 'RECON_VERSION'                           / version of event reconstruction software
TFORM10     = 'I'                                       / data format of field: 2-byte signed INTEGER
TLMIN10     = 0                                         / minimum value
TLMAX10     = 32767                                     / maximum value
TTYPE11     = 'CALIB_VERSION'                           / versions of calibration tables for the ACD, CAL, TKR
TFORM11     = '3I'                                      / data format of field: 2-byte signed INTEGER
TTYPE12     = 'IMGOODCALPROB'                           / classification tree probability that CAL energy is well measured
TFORM12     = 'E'                                       / data format of field: 4-byte REAL
TUNIT12     = ''                                        / physical unit of field: dimensionless
TLMIN12     = 0.0                                       / minimum value
TLMAX12     = 1.0                                       / maximum value
TTYPE13     = 'IMVERTEXPROB'                            / classification tree probability that the vertex gives a better measure of the direction than the best track alone
TFORM13     = 'E'                                       / data format of field: 4-byte REAL
TUNIT13     = ''                                        / physical unit of field: dimensionless
TLMIN13     = 0.0                                       / minimum value
TLMAX13     = 1.0                                       / maximum value
TTYPE14     = 'IMCOREPROB'                              / classification tree probability that the event is in the core of the PSF
TFORM14     = 'E'                                       / data format of field: 4-byte REAL
TUNIT14     = ''                                        / physical unit of field: dimensionless
TLMIN14     = 0.0                                       / minimum value
TLMAX14     = 1.0                                       / maximum value
TTYPE15     = 'IMPSFERRPRED'                            / classification tree prediction of the PSF for this event, normalized to the 68% point predicted from an analytic model
TFORM15     = 'E'                                       / data format of field: 4-byte REAL
TUNIT15     = ''                                        / physical unit of field: dimensionless
TLMIN15     = 0.0                                       / minimum value
TLMAX15     = 100.0                                     / maximum value
TTYPE16     = 'CALENERGYSUM'                            / sum of the raw energies in all the crystals
TFORM16     = 'E'                                       / data format of field: 4-byte REAL
TUNIT16     = 'MeV'                                     / physical unit of field
TLMIN16     = 0.0                                       / minimum value
TLMAX16     = 1.0e+7                                    / maximum value
TTYPE17     = 'CALTOTRLN'                               / total radiation lengths integrated along trajectory of first track
TFORM17     = 'E'                                       / data format of field: 4-byte REAL
TUNIT17     = ''                                        / physical unit of field: dimensionless
TLMIN17     = 0.0                                       / minimum value
TLMAX17     = 100.0                                     / maximum value
TTYPE18     = 'IMGAMMAPROB'                             / classification tree probability that the event is a gamma-ray
TFORM18     = 'E'                                       / data format of field: 4-byte REAL
TUNIT18     = ''                                        / physical unit of field: dimensionless
TLMIN18     = 0.0                                       / minimum value
TLMAX18     = 1.0                                       / maximum value
TTYPE19     = 'CONVERSION_POINT'                        / reconstructed 3-space conversion point in instrument coordinates
TFORM19     = '3E'                                      / data format of field: 4-byte REAL
TUNIT19     = 'm'                                       / physical unit of field
TTYPE20     = 'CONVERSION_LAYER'                        / conversion layer in TKR, -1 means not in TKR
TFORM20     = 'I'                                       / data format of field: 2-byte signed INTEGER
TLMIN20     = -1                                        / minimum value
TLMAX20     = 18                                        / maximum value
TTYPE21     = 'PULSE_PHASE'                             / pulse phase of event arrival time
TFORM21     = 'D'                                       / data format of field: 8-byte DOUBLE
TUNIT21     = ''                                        / physical unit of field
TLMIN21     = 0.0                                       / minimum value
TLMAX21     = 1.0                                       / maximum value
END

XTENSION     = 'BINTABLE'                  / binary table extension
BITPIX       = 8                           / 8-bit bytes
NAXIS        = 2                           / 2-dimensional binary table
NAXIS1       =                             / width of table in bytes
NAXIS2       =                             / number of rows in table
PCOUNT       =                             / size of special data area
GCOUNT       = 1                           / one data group (required keyword)
TFIELDS      =                             / number of fields in each row
CHECKSUM     =                             / checksum for entire HDU
DATASUM      =                             / checksum for data table
TELESCOP     = 'GLAST'                     / name of telescope generating data
INSTRUME     = 'LAT'                       / name of instrument generating data
EQUINOX      = 2000.0                      / equinox for ra and dec
RADECSYS     = 'FK5'                       / world coord. system for this file (FK5 or FK4)
DATE         =                             / file creation date (YYYY-MM-DDThh:mm:ss UT)
DATE-OBS     =                             / start date and time of the observation (UTC)
DATE-END     =                             / end date and time of the observation (UTC)
EXTNAME      = 'GTI'                       / name of this binary table extension
HDUCLASS     = 'OGIP'                      / format conforms to OGIP standard
HDUCLAS1     = 'GTI'                       / extension contains good time intervals
HDUCLAS2     = 'ALL'                       / extension contains all science time
TSTART       =                             / mission time of the start of the observation
TSTOP        =                             / mission time of the end of the observation
MJDREF       = 54101.0                     / MJD corresponding to SC clock start
TIMEUNIT     = 's'                         / units for the time related keywords
TIMESYS      = 'TT'                        / type of time system that is used
TIMEREF      = 'LOCAL'                     / reference frame used for times
TASSIGN      = 'SATELLITE'                 / location where time assignment performed
CLOCKAPP     =                             / whether a clock drift correction has been applied
GPS_OUT      =                             / whether GPS time was unavailable at any time during this interval
ONTIME       =                             / sum of GTI lengths
TELAPSE      =                             / time between START of the first GTI and STOP of the last
TTYPE1       = 'START'                     / start time of good time intervals
TFORM1       = 'D'                         / data format of field: 8-byte DOUBLE
TUNIT1       = 's'                         / physical unit of field
TLMIN1       = 0.0                         / minimum value
TLMAX1       = 1.0D+10                     / maximum value
TTYPE2       = 'STOP'                      / stop time of good time intervals
TFORM2       = 'D'                         / data format of field: 8-byte DOUBLE
TUNIT2       = 's'                         / physical unit of field
TLMIN2       = 0.0                         / minimum value
TLMAX2       = 1.0D+10                     / maximum value
END
