#
# @file Makefile
# @brief Hand-coded Makefile for building the optimizers.py module
#
# @author J. Chiang
#
# $Header$
#

SWIG = swig
#SWIG_DIR = /scratch/local
#PYTHONDIR = $(GLAST_EXT)/python/2.3/Linux
#PYINCLUDE = -I$(PYTHONDIR)/include
SWIG_DIR = /usr/local
PYINCLUDE = -I/usr/local/include/python2.3

#CFLAGS = -ansi -Wall -W -pedantic
CFLAGS = -g -fPIC

INCLUDE = -I$(OPTIMIZERSROOT)\
	-I$(XMLROOT)\
	-I$(FACILITIESROOT)\
	-I$(F2CROOT)\
	-I$(CFITSIO_DIR)/include\
	-I$(CLHEP_DIR)/1.8.0.0/include\
	-I$(GLAST_EXT)/xerces/1.7.0/include\
	-I$(GLAST_EXT)/xerces/1.7.0/include/xercesc

LIBS = -L../$(BINDIR)/ -loptimizersLib\
	-L$(XMLROOT)/$(BINDIR) -lxml\
	-L$(FACILITIESROOT)/$(BINDIR) -lfacilities\
	-L$(CFITSIO_DIR)/lib -lcfitsio\
	-L$(CLHEP_DIR)/1.8.0.0/lib -lCLHEP\
	-L$(GLAST_EXT)/xerces/1.7.0/lib/ -lxerces-c1_7_0 -lpthread\
	-lg2c -L$(SWIG_DIR)/lib -lswigpy -fPIC

module_base = optimizers
module_name = _${module_base}.so

${module_name}: ${module_base}.o
	${CXX} -shared $< ${LIBS} -o ${module_name}

${module_base}.o: ${module_base}.cxx
	${CXX} -c ${CFLAGS} ${INCLUDE} ${PYINCLUDE} -o $@ $<

${module_base}.cxx: ${module_base}.i
	${SWIG} -runtime -python -c++ -o ${module_base}.cxx ${module_base}.i

anew:
	make clean
	make

tar_file:
	tar cvf ${MY_PROG}.tar ${SRCS} ${HFILES} ${OTHER_FILES} ${srcs}

clean:
	/bin/rm -f *~
	/bin/rm -f *.o
	/bin/rm -f _*.so
	/bin/rm -f *.trace
	/bin/rm -f *wrap*
	/bin/rm -f ${module_base}.cxx
	/bin/rm -f ${module_base}.py*

very_clean:
	make clean
	/bin/rm -f core
	/bin/rm -f _${module_base}.so
