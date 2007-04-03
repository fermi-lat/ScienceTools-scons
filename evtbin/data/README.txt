Generation of unit test data for evtbin
---------------------------------------

Author: Eric Winter (Eric.L.Winter.1@gsfc.nasa.gov)
Date: Friday 23 March 2007

We started with 2 files of test data from GLAST DC2:

...
  -rw-r--r--  1 elwinter glast  8850240 Mar 13 13:14 dc2-crab-15deg.fits
  -rw-r--r--  1 elwinter glast 15480000 Mar 13 13:14 DC2_FT2_v2.fits
...

The first file is a FT1 (event data) file, and the second is a FT2
(spacecraft data) file.

The purpose of this exercise is to generate a small set of event and
spacecraft data that may be used for unit testing of gtbin and other
GLAST science tools. We wanted approximately 1000 events, centered on
the Crab Nebula.

The Crab coordinates used were taken from the circle() region defined
in the FT1 file: RA = 83.633208, DEC = 22.014472, R = 15.

Step 0: Time-order the event data
---------------------------------

The events in the original FT1 file are not in time order, so order
them based on the TIME column:

...
cp dc2-crab-15deg.fits dc2-crab-15deg_time_sorted.fits
sulacco [92] [elwinter]: fsort
Name of FITS file and [ext#][dc2-crab-15deg.fits[2]] dc2-crab-15deg_time_sorted.fits
Column Names for 1st,2nd,... Sort[TIME]
Sort Algorithm(heap,insert)[heap]
...

A quick plot of TIME vs row number in the resulting FT1 file shows the
time increasing linearly, as it should. But the GTIs are probably
messed up; they were copied unchanged.

Step 1: Determine a suitable time range
---------------------------------------

Begin by examining the FT2 file with fselect to find time intervals
where the spacecraft z-axis was close (within 15 deg) to the position
of the Crab.

...
sulacco [47] [elwinter]: fselect
Name of FITS file and [ext#][../dc2/old_data/DC2_FT2_v2.fits]
Name of output FITS file[crab_ft2.fits] crab_ft2_close.fits
Selection Expression[stop>=(2.2385e8-30)&&start<=(2.2395e8+30)] ra_scz > 68.633208 && ra_scz < 98.633208 && dec_scz > 7.014472 && dec_scz < 37.014472
...

The resulting file contained 3484 spacecraft records of 30 seconds
each, for a total duration of 1742 minutes. Plotting RA_SCZ as a
function of START showed 2 time periods in which the Crab views were
clustered. I chose a 100,000 second period around the center of the
first cluster as the time period to use. The interval was
2.2385e8-2.2395e8.

Step 2: Extract event data
--------------------------

We used gtselect to extract the events in the desired time range and
sky area:

...
sulacco [28] [elwinter]: gtselect
Input FT1 file [dc2-crab-15deg_time_sorted.fits] :
Output FT1 file [crab_ft1.fits] :
RA for new search center (degrees) <0 - 360> [83.633208] :
Dec for new search center (degrees) <-90 - 90> [22.014472] :
radius of new search region (degrees) <0 - 180> [15] :
start time (MET in s) [223850000] :
end time (MET in s) [223950000] :
lower energy limit (MeV) [30] :
upper energy limit (MeV) [200000] :
Event classes (-1=all, 0=FrontA, 1=BackA, 2=FrontB, 3=BackB, 4=class A) <-1 - 4> [-1] :
Done.
...

The resulting file contained 1552 events and 13 GTIs.

Step 3: Extract spacecraft data
-------------------------------

We used fselect to extract spacecraft data records from the FT2 file,
using criteria which ensured the presence of at least 1 30-second
spacecraft record before the start of and after the end of the desired
time period:

...
sulacco [96] [elwinter]: fselect
Name of FITS file and [ext#][../dc2/old_data/DC2_FT2_v2.fits]
Name of output FITS file[crab_ft2_close.fits] crab_ft2.fits
Selection Expression[ra_scz > 68.633208 && ra_scz < 98.633208 && dec_scz > 7.014472 && dec_scz < 37.014472] stop>=(2.2385e8-30)&&start<=(2.2395e8+30)
...

The resulting file contained 3361 spacecraft data records.

At this point, we have 2 files (crab_ft1.fits and crab_ft2.fits) which
can be used for unit testing.

Step 4: Sample product generation
---------------------------------

We used gtbin to generate each possible product type with these files:

CCUBE:

...
sulacco [11] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [PHA2] : ccube
Event data file name [crab_ft1.fits] :
Output file name [crab.pha2] : crab.ccube
Spacecraft data file name [crab_ft2.fits] :
Size of the X axis in pixels [300] :
Size of the Y axis in pixels [300] :
Image scale (in degrees/pixel) [0.1] :
Coordinate system (CEL - celestial, GAL -galactic) <CEL|GAL> [CEL] :
First coordinate of image center in degrees (RA or galactic l) [83.4] :
Second coordinate of image center in degrees (DEC or galactic b) [22] :
Rotation angle of image axis, in degrees [0] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [100] : 10
...

CMAP:

...
sulacco [12] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [CCUBE] : cmap
Event data file name [crab_ft1.fits] :
Output file name [crab.ccube] : crab.cmap
Spacecraft data file name [crab_ft2.fits] :
Size of the X axis in pixels [300] :
Size of the Y axis in pixels [300] :
Image scale (in degrees/pixel) [0.1] :
Coordinate system (CEL - celestial, GAL -galactic) <CEL|GAL> [CEL] :
First coordinate of image center in degrees (RA or galactic l) [83.4] :
Second coordinate of image center in degrees (DEC or galactic b) [22] :
Rotation angle of image axis, in degrees [0] :
...

LC:

...
sulacco [13] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [CMAP] : lc
Event data file name [crab_ft1.fits] :
Output file name [crab.cmap] : crab.lc
Spacecraft data file name [crab_ft2.fits] :
Algorithm for defining time bins <FILE|LIN|SNR> [LIN] :
Start value for first time bin [223850000] :
Stop value for last time bin [223950000] :
Width of linearly uniform time bins [10000] : 1200
...

PHA1:

...
sulacco [14] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [LC] : pha1
Event data file name [crab_ft1.fits] :
Output file name [crab.lc] : crab.pha1
Spacecraft data file name [crab_ft2.fits] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [10] : 100
...

PHA2:

...
sulacco [15] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [PHA1] : pha2
Event data file name [crab_ft1.fits] :
Output file name [crab.pha1] : crab.pha2
Spacecraft data file name [crab_ft2.fits] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [100] :
Algorithm for defining time bins <FILE|LIN|SNR> [LIN] :
Start value for first time bin [223850000] :
Stop value for last time bin [223950000] :
Width of linearly uniform time bins [1200] : 10000
...

Step 5: Event subsample generation
----------------------------

Now I need to subdivide the event data into 3 files so that multi-file
aspects of the tools may be exercised. The easiest thing to do is to
simply evenly divide the records by time. The time range in
crab_ft1.fits is 223853164-223946497 (interval of 93333), so use a
time interval of 31111 for each of the smaller files.

...
sulacco [16] [elwinter]: ../ScienceTools-v7r6p1/dataSubselector/v4r4/Linux-i686/gtselect.exe
Input FT1 file [] : crab_ft1.fits
Output FT1 file [] : crab_ft1_0.fits
RA for new search center (degrees) <0 - 360> [] : 83.633208
Dec for new search center (degrees) <-90 - 90> [] : 22.014472
radius of new search region (degrees) <0 - 180> [] : 15
start time (MET in s) [] : 223850000
end time (MET in s) [] : 223880000
lower energy limit (MeV) [] : 30
upper energy limit (MeV) [] : 200000
Event classes (-1=all, 0=FrontA, 1=BackA, 2=FrontB, 3=BackB, 4=class A) <-1 - 4> [-1] :
Done.
...

...
sulacco [18] [elwinter]: ../ScienceTools-v7r6p1/dataSubselector/v4r4/Linux-i686/gtselect.exe
Input FT1 file [crab_ft1.fits] :
Output FT1 file [crab_ft1_0.fits] : crab_ft1_1.fits
RA for new search center (degrees) <0 - 360> [83.633208] :
Dec for new search center (degrees) <-90 - 90> [22.014472] :
radius of new search region (degrees) <0 - 180> [15] :
start time (MET in s) [223853164] : 223880000
end time (MET in s) [223884275] : 223910000
lower energy limit (MeV) [30] :
upper energy limit (MeV) [200000] :
Event classes (-1=all, 0=FrontA, 1=BackA, 2=FrontB, 3=BackB, 4=class A) <-1 - 4> [-1] :
Done.
...

...
sulacco [19] [elwinter]: ../ScienceTools-v7r6p1/dataSubselector/v4r4/Linux-i686/gtselect.exe
Input FT1 file [crab_ft1.fits] :
Output FT1 file [crab_ft1_1.fits] : crab_ft1_2.fits
RA for new search center (degrees) <0 - 360> [83.633208] :
Dec for new search center (degrees) <-90 - 90> [22.014472] :
radius of new search region (degrees) <0 - 180> [15] :
start time (MET in s) [223884275] : 223910000
end time (MET in s) [223915386] : 223950000
lower energy limit (MeV) [30] :
upper energy limit (MeV) [200000] :
Event classes (-1=all, 0=FrontA, 1=BackA, 2=FrontB, 3=BackB, 4=class A) <-1 - 4> [-1] :
Done.
...

The files crab_ft1_?.fits were then listed in ft1.lst.

Step 6: Spacecraft data subsample generation
--------------------------------------------

Now I need to subdivide the spacecraft data using the same intervals
as the event data. Use fselect.


...
sulacco [23] [elwinter]: fselect
Name of FITS file and [ext#][crab_ft2.fits]
Name of output FITS file[crab_ft2_0.fits]
Selection Expression[] start < 223880010
...

...
sulacco [31] [elwinter]: fselect
Name of FITS file and [ext#][crab_ft2.fits]
Name of output FITS file[crab_ft2_1.fits]
Selection Expression[] stop >= 223880010 && stop < 223910010
...

...
sulacco [32] [elwinter]: fselect
Name of FITS file and [ext#][crab_ft2.fits]
Name of output FITS file[crab_ft2_1.fits] crab_ft2_2.fits
Selection Expression[] stop > 223910010
...

The files crab_ft2_?.fits were then listed in ft2.lst.

Step 7: Sample product generation from subsetted files
------------------------------------------------------

CCUBE:

...
sulacco [7] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [PHA2] : ccube
Event data file name [@ft1.lst] :
Output file name [crab_012.pha2] : crab_012.ccube
Spacecraft data file name [@ft2.lst] :
Size of the X axis in pixels [300] :
Size of the Y axis in pixels [300] :
Image scale (in degrees/pixel) [0.1] :
Coordinate system (CEL - celestial, GAL -galactic) <CEL|GAL> [CEL] :
First coordinate of image center in degrees (RA or galactic l) [83.4] :
Second coordinate of image center in degrees (DEC or galactic b) [22] :
Rotation angle of image axis, in degrees [0] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [10] :
...

CMAP:

...
sulacco [8] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [CCUBE] : cmap
Event data file name [@ft1.lst] :
Output file name [crab_012.ccube] : crab_012.cmap
Spacecraft data file name [@ft2.lst] :
Size of the X axis in pixels [300] :
Size of the Y axis in pixels [300] :
Image scale (in degrees/pixel) [0.1] :
Coordinate system (CEL - celestial, GAL -galactic) <CEL|GAL> [CEL] :
First coordinate of image center in degrees (RA or galactic l) [83.4] :
Second coordinate of image center in degrees (DEC or galactic b) [22] :
Rotation angle of image axis, in degrees [0] :
...

LC:

...
sulacco [9] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [CMAP] : lc
Event data file name [@ft1.lst] :
Output file name [crab_012.cmap] : crab_012.lc
Spacecraft data file name [@ft2.lst] :
Algorithm for defining time bins <FILE|LIN|SNR> [LIN] :
Start value for first time bin [223850000] :
Stop value for last time bin [223950000] :
Width of linearly uniform time bins [10] : 1200
...

PHA1:

...
sulacco [10] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [LC] : pha1
Event data file name [@ft1.lst] :
Output file name [crab_012.lc] : crab_012.pha1
Spacecraft data file name [@ft2.lst] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [10] : 100
...

PHA2:

...
sulacco [11] [elwinter]: ../ScienceTools-v7r6p1/evtbin/v1/Linux-i686/gtbin.exe
This is gtbin version v1
Type of output file <CCUBE|CMAP|LC|PHA1|PHA2> [PHA1] : pha2
Event data file name [@ft1.lst] :
Output file name [crab_012.pha1] : crab_012.pha2
Spacecraft data file name [@ft2.lst] :
Algorithm for defining energy bins <FILE|LIN|LOG> [LOG] :
Start value for first energy bin [30] :
Stop value for last energy bin [200000] :
Number of logarithmically uniform energy bins [100] :
Algorithm for defining time bins <FILE|LIN|SNR> [LIN] :
Start value for first time bin [223850000] :
Stop value for last time bin [223950000] :
Width of linearly uniform time bins [1200] : 10000
...
