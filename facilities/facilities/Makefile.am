## Process this file with automake to produce Makefile.in
#
# $Id$
#
# Author: Paul_Kunz@slac.stanford.edu
#

# The following set,  else would follow GNU conventions
AUTOMAKE_OPTIONS = foreign

libfacilities_ladir = $(includedir)/facilities

## headers required in shared file system installation
libfacilities_la_HEADERS =     \
	Adapter.h        \
	Clock.h          \
	FIFO.h           \
	Observer.h       \
	Scheduler.h      \
	ScheduledEvent.h \
	SimpleEvent.h    \
	Utility.h        \
	bitmanip.h       \
	bpkt_streams.h   \
	error.h



## headers not needed in shared file system installation
noinst_HEADERS =         \
	Commissioner.h   \
	Converter.h      \
	binarystream.h   \
	filemap.h
