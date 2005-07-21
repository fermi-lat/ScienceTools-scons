#!/bin/tcsh -f
#
# Testrun 1
#
set    RUN_ID = "test1"
setenv PFILES ../../pfiles
#
# Find Cygnus region 3EG counterparts in ROSAT catalogue
#=======================================================
gtsrcid.exe \
  srcCatName="../../data/cygob2_egret_3eg.tsv" \
  srcCatQty="3EG,RAJ2000,DEJ2000,theta95" \
  cptCatName="../../data/cygob2_rosat_1rxs.tsv" \
  cptCatQty="1RXS,RAJ2000,DEJ2000,PosErr" \
  outCatName="../../data/outref/${RUN_ID}.fits" \
  probMethod="POSITION" \
  probThres="0.05" \
  maxNumCtp="100" \
  chatter="4" \
  clobber="yes" \
  debug="no" \
  mode="q" >& "../../data/outref/$RUN_ID.dump"
mv gtsrcid.log "../../data/outref/$RUN_ID.log"
