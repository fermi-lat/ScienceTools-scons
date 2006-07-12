CMT_tag=$(tag)
CMTROOT=/data0/glast/extlib/rh9_gcc32/CMT/v1r16p20040701
CMT_root=/data0/glast/extlib/rh9_gcc32/CMT/v1r16p20040701
CMTVERSION=v1r16p20040701
CMTrelease=15
cmt_hardware_query_command=uname -m
cmt_hardware=`$(cmt_hardware_query_command)`
cmt_system_version_query_command=${CMTROOT}/mgr/cmt_linux_version.sh | ${CMTROOT}/mgr/cmt_filter_version.sh
cmt_system_version=`$(cmt_system_version_query_command)`
cmt_compiler_version_query_command=${CMTROOT}/mgr/cmt_gcc_version.sh | ${CMTROOT}/mgr/cmt_filter_version.sh
cmt_compiler_version=`$(cmt_compiler_version_query_command)`
PATH=/data0/glast/extlib/rh9_gcc32/CMT/v1r16p20040701/${CMTBIN}:/data33/glast/ground/GLAST_EXT/rh9_gcc32/python/2.4.1/bin:/usr/kerberos/bin:/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:/usr/X11R6/bin:/data0/glast/IDL/idl/bin:/data0/glast/extlib/ruby/bin:/home/users/omodei/myScripts:/usr/kerberos/bin:/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:/usr/X11R6/bin:/data0/glast/IDL/idl/bin:/data0/glast/extlib/ruby/bin:/home/users/omodei/myScripts:/usr/kerberos/bin:/usr/local/bin:/usr/bin:/bin:/usr/X11R6/bin:/usr/X11R6/bin:.:/cern/pro/bin:/afs/pi.infn.it/sw/root/pro/i386_linux24_3.2/root/bin:/home/users/omodei/bin:/usr/X11R6/bin:.:/cern/pro/bin:/afs/pi.infn.it/sw/root/pro/i386_linux24_3.2/root/bin:${ROOTSYS}/bin
CLASSPATH=/data0/glast/extlib/rh9_gcc32/CMT/v1r16p20040701/java
debug_option=-g
cc=gcc
cdebugflags=$(debug_option)
pp_cflags=-Di586
ccomp=$(cc) -c $(includes) $(cdebugflags) $(cflags) $(pp_cflags)
clink=$(cc) $(clinkflags) $(cdebugflags)
ppcmd=-I
preproc=c++ -MD -c 
cpp=g++
cppdebugflags=$(debug_option)
cppflags=-pipe -ansi -W -Wall  -fPIC -shared -D_GNU_SOURCE -Dlinux -Dunix 
pp_cppflags=-D_GNU_SOURCE
cppcomp=$(cpp) -c $(includes) $(cppoptions) $(cppflags) $(pp_cppflags)
cpplinkflags=-Wl,-Bdynamic  $(linkdebug)
cpplink=$(cpp)   $(cpplinkflags)
for=g77
fflags=$(debug_option)
fcomp=$(for) -c $(fincludes) $(fflags) $(pp_fflags)
flink=$(for) $(flinkflags)
javacomp=javac -classpath $(src):$(CLASSPATH) 
javacopy=cp
jar=jar
X11_cflags=-I/usr/include
Xm_cflags=-I/usr/include
X_linkopts=-L/usr/X11R6/lib -lXm -lXt -lXext -lX11 -lm
lex=flex $(lexflags)
yaccflags= -l -d 
yacc=yacc $(yaccflags)
ar=ar r
ranlib=ranlib
make_shlib=${CMTROOT}/mgr/cmt_make_shlib_common.sh extract
shlibsuffix=so
shlibbuilder=g++ $(cmt_installarea_linkopts) 
shlibflags=-shared
symlink=/bin/ln -fs 
symunlink=/bin/rm -f 
build_library_links=$(cmtexe) build library_links -quiet -tag=$(tags)
remove_library_links=$(cmtexe) remove library_links -quiet -tag=$(tags)
cmtexe=${CMTROOT}/${CMTBIN}/cmt.exe
build_prototype=$(cmtexe) build prototype
build_dependencies=$(cmtexe) -quiet -tag=$(tags) build dependencies
build_triggers=$(cmtexe) build triggers
implied_library_prefix=-l
SHELL=/bin/sh
src=../src/
doc=../doc/
inc=../src/
mgr=../cmt/
application_suffix=.exe
library_prefix=lib
lock_command=chmod -R a-w ../*
unlock_command=chmod -R g+w ../*
MAKEFLAGS= --no-print-directory 
gmake_hosts=lx1 rsplus lxtest as7 dxplus ax7 hp2 aleph hp1 hpplus papou1-fe atlas
make_hosts=virgo-control1 rio0a vmpc38a
everywhere=hosts
install_command=cp 
uninstall_command=/bin/rm -f 
cmt_installarea_command=ln -s 
cmt_uninstallarea_command=/bin/rm -f 
cmt_install_area_command=$(cmt_installarea_command)
cmt_uninstall_area_command=$(cmt_uninstallarea_command)
cmt_install_action=$(CMTROOT)/mgr/cmt_install_action.sh
cmt_installdir_action=$(CMTROOT)/mgr/cmt_installdir_action.sh
cmt_uninstall_action=$(CMTROOT)/mgr/cmt_uninstall_action.sh
cmt_uninstalldir_action=$(CMTROOT)/mgr/cmt_uninstalldir_action.sh
mkdir=mkdir
cmt_installarea_prefix=InstallArea
CMT_PATH_remove_regexp=/[^/]*/
CMT_PATH_remove_share_regexp=/share/
NEWCMTCONFIG=i686-unknown00-gcc32
GRBtemplate_tag=$(tag)
GRBTEMPLATEROOT=/data0/glast/ScienceTools/celestialSources/GRBtemplate/v0r2
GRBtemplate_root=/data0/glast/ScienceTools/celestialSources/GRBtemplate/v0r2
GRBTEMPLATEVERSION=v0r2
GRBtemplate_cmtpath=/data0/glast/ScienceTools
GRBtemplate_offset=celestialSources
GRBtemplate_project=Project1
flux_tag=$(tag)
FLUXROOT=/data0/glast/ScienceTools/flux/v8r23
flux_root=/data0/glast/ScienceTools/flux/v8r23
FLUXVERSION=v8r23
flux_cmtpath=/data0/glast/ScienceTools
flux_project=Project1
xmlBase_tag=$(tag)
XMLBASEROOT=/data0/glast/ScienceTools/xmlBase/v5r3
xmlBase_root=/data0/glast/ScienceTools/xmlBase/v5r3
XMLBASEVERSION=v5r3
xmlBase_cmtpath=/data0/glast/ScienceTools
xmlBase_project=Project1
GlastPolicy_tag=$(tag)
GLASTPOLICYROOT=/data0/glast/ScienceTools/GlastPolicy/v6r4
GlastPolicy_root=/data0/glast/ScienceTools/GlastPolicy/v6r4
GLASTPOLICYVERSION=v6r4
GlastPolicy_cmtpath=/data0/glast/ScienceTools
GlastPolicy_project=Project1
GlastPatternPolicy_tag=$(tag)
GLASTPATTERNPOLICYROOT=/data0/glast/ScienceTools/GlastPolicy/GlastPatternPolicy/v0r1
GlastPatternPolicy_root=/data0/glast/ScienceTools/GlastPolicy/GlastPatternPolicy/v0r1
GLASTPATTERNPOLICYVERSION=v0r1
GlastPatternPolicy_cmtpath=/data0/glast/ScienceTools
GlastPatternPolicy_offset=GlastPolicy
GlastPatternPolicy_project=Project1
GlastMain=${GLASTPOLICYROOT}/src/GlastMain.cxx
TestGlastMain=${GLASTPOLICYROOT}/src/TestGlastMain.cxx
GlastCppPolicy_tag=$(tag)
GLASTCPPPOLICYROOT=/data0/glast/ScienceTools/GlastPolicy/GlastCppPolicy/v0r2p3
GlastCppPolicy_root=/data0/glast/ScienceTools/GlastPolicy/GlastCppPolicy/v0r2p3
GLASTCPPPOLICYVERSION=v0r2p3
GlastCppPolicy_cmtpath=/data0/glast/ScienceTools
GlastCppPolicy_offset=GlastPolicy
GlastCppPolicy_project=Project1
BINDIR=rh9_gcc32
cppoptions=$(cppdebugflags_s)
cppdebugflags_s=-g
cppoptimized_s=-O2
cppprofiled_s=-pg
linkdebug=-g 
makeLinkMap=-Wl,-Map,Linux.map
componentshr_linkopts=-fPIC  -ldl 
libraryshr_linkopts=-fPIC -ldl 
XMLEXT_tag=$(tag)
XMLEXTROOT=/data0/glast/ScienceTools/IExternal/XMLEXT/v5r260p0
XMLEXT_root=/data0/glast/ScienceTools/IExternal/XMLEXT/v5r260p0
XMLEXTVERSION=v5r260p0
XMLEXT_cmtpath=/data0/glast/ScienceTools
XMLEXT_offset=IExternal
XMLEXT_project=Project1
EXTPACK_DIR=${GLAST_EXT}
XMLEXT_native_version=2.6.0
XMLEXT_DIR=${EXTPACK_DIR}/xerces/$(XMLEXT_native_version)
LD_LIBRARY_PATH=/data0/glast/extlib/ruby/lib:/afs/pi.infn.it/sw/root/pro/i386_linux24_3.2/root/lib:/data33/glast/ground/GLAST_EXT/rh9_gcc32/cfitsio/v2470/lib:/data33/glast/ground/GLAST_EXT/rh9_gcc32/cppunit/1.9.14/lib:/data33/glast/ground/GLAST_EXT/rh9_gcc32/fftw/3.0.1/lib:/data0/glast/ScienceTools/st_graph/v1r5p7/rh9_gcc32:/data0/glast/ScienceTools/irfs/testResponse/v0r4p2/rh9_gcc32:/data0/glast/ScienceTools/irfs/rootIrfLoader/v0/rh9_gcc32:/data0/glast/ScienceTools/optimizers/v2r5p2/rh9_gcc32:/data33/glast/ground/GLAST_EXT/rh9_gcc32/xerces/2.6.0/lib:/data0/glast/ScienceTools/xmlBase/v5r3/${BINDIR}:/data33/glast/ground/GLAST_EXT/rh9_gcc32/CLHEP/1.8.0.0/lib:/data33/glast/ground/GLAST_EXT/rh9_gcc32/ROOT/v4.02.00/root/bin:/data33/glast/ground/GLAST_EXT/rh9_gcc32/ROOT/v4.02.00/root/lib
XMLEXT_linkopts=-L$(XMLEXT_DIR)/lib/ -lxerces-c -lpthread
facilities_tag=$(tag)
FACILITIESROOT=/data0/glast/ScienceTools/facilities/v2r12p4
facilities_root=/data0/glast/ScienceTools/facilities/v2r12p4
FACILITIESVERSION=v2r12p4
facilities_cmtpath=/data0/glast/ScienceTools
facilities_project=Project1
facilities_linkopts=-L${facilities_root}/${BINDIR} -lfacilities 
xmlBase_linkopts=-L${xmlBase_root}/${BINDIR} -lxmlBase 
xmlBase_shlibflags=$(libraryshr_linkopts)
xmlBase_stamps=${XMLBASEROOT}/${BINDIR}/xmlBase.stamp 
CLHEP_tag=$(tag)
CLHEPROOT=/data0/glast/ScienceTools/IExternal/CLHEP/v2r1800p5
CLHEP_root=/data0/glast/ScienceTools/IExternal/CLHEP/v2r1800p5
CLHEPVERSION=v2r1800p5
CLHEP_cmtpath=/data0/glast/ScienceTools
CLHEP_offset=IExternal
CLHEP_project=Project1
CLHEP_native_version=1.8.0.0
CLHEP_DIR=$(GLAST_EXT)/CLHEP
CLHEPBASE=${CLHEP_DIR}/$(CLHEP_native_version)
CLHEP_linkopts=-L$(CLHEPBASE)/lib -lCLHEP
astro_tag=$(tag)
ASTROROOT=/data0/glast/ScienceTools/astro/v1r15
astro_root=/data0/glast/ScienceTools/astro/v1r15
ASTROVERSION=v1r15
astro_cmtpath=/data0/glast/ScienceTools
astro_project=Project1
cfitsio_tag=$(tag)
CFITSIOROOT=/data0/glast/ScienceTools/IExternal/cfitsio/v1r2470p5
cfitsio_root=/data0/glast/ScienceTools/IExternal/cfitsio/v1r2470p5
CFITSIOVERSION=v1r2470p5
cfitsio_cmtpath=/data0/glast/ScienceTools
cfitsio_offset=IExternal
cfitsio_project=Project1
cfitsio_native_version=v2470
cfitsio_DIR=${GLAST_EXT}/cfitsio/$(cfitsio_native_version)
cfitsio_libs=-L${cfitsio_DIR}/lib -lcfitsio 
cfitsio_linkopts=$(cfitsio_libs) 
extFiles_tag=$(tag)
EXTFILESROOT=/data0/glast/ScienceTools/IExternal/extFiles/v0r5
extFiles_root=/data0/glast/ScienceTools/IExternal/extFiles/v0r5
EXTFILESVERSION=v0r5
extFiles_cmtpath=/data0/glast/ScienceTools
extFiles_offset=IExternal
extFiles_project=Project1
extFiles_DIR=${GLAST_EXT}/extFiles
extFiles_native_version=v0r5
extFiles_PATH=${extFiles_DIR}/$(extFiles_native_version)
EXTFILESSYS=/data33/glast/ground/GLAST_EXT/rh9_gcc32/extFiles/v0r5
tip_tag=$(tag)
TIPROOT=/data0/glast/ScienceTools/tip/v2r9
tip_root=/data0/glast/ScienceTools/tip/v2r9
TIPVERSION=v2r9
tip_cmtpath=/data0/glast/ScienceTools
tip_project=Project1
STpolicy_tag=$(tag)
STPOLICYROOT=/data0/glast/ScienceTools/STpolicy/v1r1
STpolicy_root=/data0/glast/ScienceTools/STpolicy/v1r1
STPOLICYVERSION=v1r1
STpolicy_cmtpath=/data0/glast/ScienceTools
STpolicy_project=Project1
TMP=/tmp
ROOT_tag=$(tag)
ROOTROOT=/data0/glast/ScienceTools/IExternal/ROOT/v3r40200p4
ROOT_root=/data0/glast/ScienceTools/IExternal/ROOT/v3r40200p4
ROOTVERSION=v3r40200p4
ROOT_cmtpath=/data0/glast/ScienceTools
ROOT_offset=IExternal
ROOT_project=Project1
ROOT_DIR=${GLAST_EXT}/ROOT
ROOT_native_version=v4.02.00
ROOT_PATH=${ROOT_DIR}/$(ROOT_native_version)/root
ROOTSYS=/data33/glast/ground/GLAST_EXT/rh9_gcc32/ROOT/v4.02.00/root
dict=../dict/
rootcint=rootcint
ROOT_libs=-L$(ROOT_PATH)/lib -lCore -lCint -lTree -lMatrix -lPhysics -lpthread -lm -ldl -rdynamic -lHist -lGraf  -lGpad 
ROOT_GUI_libs=-L$(ROOT_PATH)/lib -lHist -lGraf -lGraf3d -lGpad -lRint -lPostscript -lTreePlayer 
ROOT_linkopts=$(ROOT_libs)
ROOT_cppflagsEx=$(ppcmd) "$(ROOT_PATH)/include" -DUSE_ROOT
ROOT_cppflags=-fpermissive
tip_linkopts=-lHist -L${tip_root}/${BINDIR} -ltip 
tipDir=${TIPROOT}/${BINDIR}
tip_stamps=${TIPROOT}/${BINDIR}/tip.stamp 
astro_linkopts=-L${astro_root}/${BINDIR} -lastro 
astro_stamps=${ASTROROOT}/${BINDIR}/astro.stamp 
flux_linkopts=-L${flux_root}/${BINDIR} -lflux 
flux_stamps=${FLUXROOT}/${BINDIR}/flux.stamp 
FLUX_XML=/data0/glast/ScienceTools/flux/v8r23/xml
SpectObj_tag=$(tag)
SPECTOBJROOT=/data0/glast/ScienceTools/celestialSources/SpectObj/v1r1p6
SpectObj_root=/data0/glast/ScienceTools/celestialSources/SpectObj/v1r1p6
SPECTOBJVERSION=v1r1p6
SpectObj_cmtpath=/data0/glast/ScienceTools
SpectObj_offset=celestialSources
SpectObj_project=Project1
eblAtten_tag=$(tag)
EBLATTENROOT=/data0/glast/ScienceTools/celestialSources/eblAtten/v0r3p1
eblAtten_root=/data0/glast/ScienceTools/celestialSources/eblAtten/v0r3p1
EBLATTENVERSION=v0r3p1
eblAtten_cmtpath=/data0/glast/ScienceTools
eblAtten_offset=celestialSources
eblAtten_project=Project1
eblAtten_linkopts=-L${eblAtten_root}/${BINDIR} -leblAtten 
eblAtten_stamps=${EBLATTENROOT}/${BINDIR}/eblAtten.stamp 
SpectObj_linkopts=-L${SpectObj_root}/${BINDIR} -lSpectObj 
GRBtemplate_linkopts=-L${GRBtemplate_root}/${BINDIR} -lGRBtemplate 
source=*.cxx
GlastPatternPolicyDir=${GLASTPATTERNPOLICYROOT}/${BINDIR}
GlastPolicyDir=${GLASTPOLICYROOT}/${BINDIR}
facilitiesDir=${FACILITIESROOT}/${BINDIR}
xmlBaseDir=${XMLBASEROOT}/${BINDIR}
STpolicyDir=${STPOLICYROOT}/${BINDIR}
astroDir=${ASTROROOT}/${BINDIR}
fluxDir=${FLUXROOT}/${BINDIR}
eblAttenDir=${EBLATTENROOT}/${BINDIR}
SpectObjDir=${SPECTOBJROOT}/${BINDIR}
GRBtemplateDir=${GRBTEMPLATEROOT}/${BINDIR}
tag=rh9_gcc32
package=GRBtemplate
version=v0r2
PACKAGE_ROOT=$(GRBTEMPLATEROOT)
srcdir=../src
bin=../$(GRBtemplate_tag)/
javabin=../classes/
mgrdir=cmt
project=Project1
use_requirements=requirements $(CMTROOT)/mgr/requirements $(FLUXROOT)/cmt/requirements $(XMLBASEROOT)/cmt/requirements $(ASTROROOT)/cmt/requirements $(FACILITIESROOT)/cmt/requirements $(TIPROOT)/cmt/requirements $(STPOLICYROOT)/cmt/requirements $(SPECTOBJROOT)/cmt/requirements $(EBLATTENROOT)/cmt/requirements $(GLASTPOLICYROOT)/cmt/requirements $(GLASTPATTERNPOLICYROOT)/cmt/requirements $(GLASTCPPPOLICYROOT)/cmt/requirements $(XMLEXTROOT)/cmt/requirements $(CLHEPROOT)/cmt/requirements $(CFITSIOROOT)/cmt/requirements $(EXTFILESROOT)/cmt/requirements $(ROOTROOT)/cmt/requirements 
use_includes= $(ppcmd)"$(FLUXROOT)" $(ppcmd)"$(XMLBASEROOT)" $(ppcmd)"$(ASTROROOT)" $(ppcmd)"$(FACILITIESROOT)" $(ppcmd)"$(TIPROOT)" $(ppcmd)"$(SPECTOBJROOT)" $(ppcmd)"$(EBLATTENROOT)" $(ppcmd)"$(XMLEXT_DIR)/include" $(ppcmd)"$(CLHEPBASE)/include" $(ppcmd)"${cfitsio_DIR}/include" $(ppcmd)"$(ROOT_PATH)/include" 
use_fincludes= $(use_includes)
use_stamps= $(GRBtemplate_stamps)  $(flux_stamps)  $(xmlBase_stamps)  $(astro_stamps)  $(facilities_stamps)  $(tip_stamps)  $(STpolicy_stamps)  $(SpectObj_stamps)  $(eblAtten_stamps)  $(GlastPolicy_stamps)  $(GlastPatternPolicy_stamps)  $(GlastCppPolicy_stamps)  $(XMLEXT_stamps)  $(CLHEP_stamps)  $(cfitsio_stamps)  $(extFiles_stamps)  $(ROOT_stamps) 
use_cflags=  $(GRBtemplate_cflags)  $(flux_cflags)  $(xmlBase_cflags)  $(astro_cflags)  $(facilities_cflags)  $(tip_cflags)  $(STpolicy_cflags)  $(SpectObj_cflags)  $(eblAtten_cflags)  $(GlastPolicy_cflags)  $(XMLEXT_cflags)  $(CLHEP_cflags)  $(cfitsio_cflags)  $(extFiles_cflags)  $(ROOT_cflags) 
use_pp_cflags=  $(GRBtemplate_pp_cflags)  $(flux_pp_cflags)  $(xmlBase_pp_cflags)  $(astro_pp_cflags)  $(facilities_pp_cflags)  $(tip_pp_cflags)  $(STpolicy_pp_cflags)  $(SpectObj_pp_cflags)  $(eblAtten_pp_cflags)  $(GlastPolicy_pp_cflags)  $(XMLEXT_pp_cflags)  $(CLHEP_pp_cflags)  $(cfitsio_pp_cflags)  $(extFiles_pp_cflags)  $(ROOT_pp_cflags) 
use_cppflags=  $(GRBtemplate_cppflags)  $(flux_cppflags)  $(xmlBase_cppflags)  $(astro_cppflags)  $(facilities_cppflags)  $(tip_cppflags)  $(STpolicy_cppflags)  $(SpectObj_cppflags)  $(eblAtten_cppflags)  $(GlastPolicy_cppflags)  $(XMLEXT_cppflags)  $(CLHEP_cppflags)  $(cfitsio_cppflags)  $(extFiles_cppflags)  $(ROOT_cppflags) 
use_pp_cppflags=  $(GRBtemplate_pp_cppflags)  $(flux_pp_cppflags)  $(xmlBase_pp_cppflags)  $(astro_pp_cppflags)  $(facilities_pp_cppflags)  $(tip_pp_cppflags)  $(STpolicy_pp_cppflags)  $(SpectObj_pp_cppflags)  $(eblAtten_pp_cppflags)  $(GlastPolicy_pp_cppflags)  $(XMLEXT_pp_cppflags)  $(CLHEP_pp_cppflags)  $(cfitsio_pp_cppflags)  $(extFiles_pp_cppflags)  $(ROOT_pp_cppflags) 
use_fflags=  $(GRBtemplate_fflags)  $(flux_fflags)  $(xmlBase_fflags)  $(astro_fflags)  $(facilities_fflags)  $(tip_fflags)  $(STpolicy_fflags)  $(SpectObj_fflags)  $(eblAtten_fflags)  $(GlastPolicy_fflags)  $(XMLEXT_fflags)  $(CLHEP_fflags)  $(cfitsio_fflags)  $(extFiles_fflags)  $(ROOT_fflags) 
use_pp_fflags=  $(GRBtemplate_pp_fflags)  $(flux_pp_fflags)  $(xmlBase_pp_fflags)  $(astro_pp_fflags)  $(facilities_pp_fflags)  $(tip_pp_fflags)  $(STpolicy_pp_fflags)  $(SpectObj_pp_fflags)  $(eblAtten_pp_fflags)  $(GlastPolicy_pp_fflags)  $(XMLEXT_pp_fflags)  $(CLHEP_pp_fflags)  $(cfitsio_pp_fflags)  $(extFiles_pp_fflags)  $(ROOT_pp_fflags) 
use_linkopts= $(cmt_installarea_linkopts)   $(GRBtemplate_linkopts)  $(flux_linkopts)  $(xmlBase_linkopts)  $(astro_linkopts)  $(facilities_linkopts)  $(tip_linkopts)  $(STpolicy_linkopts)  $(SpectObj_linkopts)  $(eblAtten_linkopts)  $(GlastPolicy_linkopts)  $(XMLEXT_linkopts)  $(CLHEP_linkopts)  $(cfitsio_linkopts)  $(extFiles_linkopts)  $(ROOT_linkopts) 
use_libraries= $(flux_libraries)  $(xmlBase_libraries)  $(astro_libraries)  $(facilities_libraries)  $(tip_libraries)  $(STpolicy_libraries)  $(SpectObj_libraries)  $(eblAtten_libraries)  $(GlastPolicy_libraries)  $(GlastPatternPolicy_libraries)  $(GlastCppPolicy_libraries)  $(XMLEXT_libraries)  $(CLHEP_libraries)  $(cfitsio_libraries)  $(extFiles_libraries)  $(ROOT_libraries) 
includes= $(ppcmd)"$(GRBTEMPLATEROOT)" $(use_includes)
fincludes= $(includes)
GRBtemplate_use_linkopts=  $(GRBtemplate_linkopts)  $(flux_linkopts)  $(xmlBase_linkopts)  $(astro_linkopts)  $(facilities_linkopts)  $(tip_linkopts)  $(STpolicy_linkopts)  $(SpectObj_linkopts)  $(eblAtten_linkopts)  $(GlastPolicy_linkopts)  $(XMLEXT_linkopts)  $(CLHEP_linkopts)  $(cfitsio_linkopts)  $(extFiles_linkopts)  $(ROOT_linkopts) 
test_GRBTEMPLATEROOT_use_linkopts=  $(GRBtemplate_linkopts)  $(flux_linkopts)  $(xmlBase_linkopts)  $(astro_linkopts)  $(facilities_linkopts)  $(tip_linkopts)  $(STpolicy_linkopts)  $(SpectObj_linkopts)  $(eblAtten_linkopts)  $(GlastPolicy_linkopts)  $(XMLEXT_linkopts)  $(CLHEP_linkopts)  $(cfitsio_linkopts)  $(extFiles_linkopts)  $(ROOT_linkopts) 
constituents= GRBtemplate test_GRBTEMPLATEROOT 
all_constituents= $(constituents)
constituentsclean= test_GRBTEMPLATEROOTclean GRBtemplateclean 
all_constituentsclean= $(constituentsclean)
cmt_installarea_paths=$(cmt_installarea_prefix)/$(tag)/bin $(cmt_installarea_prefix)/$(tag)/lib $(cmt_installarea_prefix)/share/lib $(cmt_installarea_prefix)/share/bin
