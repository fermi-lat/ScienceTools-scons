## Process this file with automake to produce Makefile.in
#
# $Id$
#
# Author: Paul_Kunz@slac.stanford.edu
#

# The following set,  else would follow GNU conventions
AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libfacilities.la

libfacilities_la_SOURCES = \
	Clock.cxx          \
	ScheduledEvent.cxx \
	Scheduler.cxx

libfacilities_la_LDFLAGS = -version-info 1

INCLUDES = -I$(top_srcdir)
