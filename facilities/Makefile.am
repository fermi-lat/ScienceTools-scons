## Process this file with automake to produce Makefile.in
#
# $Id$
#
# Author: Paul_Kunz@slac.stanford.edu
#

# The following set,  else would follow GNU conventions
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = config

SUBDIRS = facilities src

lib_LTLIBRARIES = libfacilities.la

libfacilities_la_SOURCES = version.cxx

libfacilities_la_LIBADD = src/libfacilities.la

libfacilities_la_LDFLAGS = -version-info 1

