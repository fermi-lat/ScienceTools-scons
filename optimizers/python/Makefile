#
# @file Makefile
# @brief Hand-coded Makefile for building the optimizers.py module
#
# @author J. Chiang
#
# $Header$
#

#CXX = /usr/local/bin/g++
#SWIG = /u1/local/bin/swig
SWIG = swig
pypath = /usr/local

INCLUDE = -I$(CMTPATH)/optimizers/v0r1\
	-I$(CMTPATH)/xml/v4r3p0/h\
	-I$(CMTPATH)/facilities/v2r7p2\
	-I$(CMTPATH)/f2c/v2\
	-I$(GLAST_EXT)/cfitsio/v2410/include\
	-I$(GLAST_EXT)/CLHEP/1.8.0.0/include\
	-I$(GLAST_EXT)/xerces/1.7.0/include\
	-I$(GLAST_EXT)/xerces/1.7.0/include/xercesc

PYINCLUDE = -I$(pypath)/include/python2.2

LIBS = -L../rh72_gcc2953/ -loptimizers\
	-L$(CMTPATH)/xml/v4r3p0/rh72_gcc2953 -lxml\
	-L$(CMTPATH)/facilities/v2r7p2/rh72_gcc2953 -lfacilities\
	-L$(GLAST_EXT)/cfitsio/v2410/lib -lcfitsio\
	-L$(GLAST_EXT)/CLHEP/1.8.0.0/lib -lCLHEP\
	-L$(GLAST_EXT)/xerces/1.7.0/lib/ -lxerces-c1_7_0 -lpthread\
	-lg2c

module_base = optimizers
module_name = _${module_base}.so

${module_name}: ${module_base}_wrap.o
	${CXX} -shared $< ${LIBS} -o ${module_name}

${module_base}_wrap.o: ${module_base}_wrap.cxx
	${CXX} -c ${INCLUDE} ${PYINCLUDE} -o $@ $<

${module_base}_wrap.cxx: ${module_base}.i ${HFILES}
	${SWIG} -python -c++ ${module_base}.i

anew:
	make clean
	make

tar_file:
	tar cvf ${MY_PROG}.tar ${SRCS} ${HFILES} ${OTHER_FILES} ${srcs}

clean:
	/bin/rm -f *~
	/bin/rm -f *.o
	/bin/rm -f *.trace
#	/bin/rm -f *wrap*

very_clean:
	make clean
	/bin/rm -f core
	/bin/rm -f _${module_base}.so
