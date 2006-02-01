#!/bin/tcsh -f
#
# Testrun 1
#
set    RUN_ID = "test1"
setenv PFILES ../../pfiles
#
# Find 3EG counterparts in North 20 cm survey catalogue of White et al. 1992
#===========================================================================
../../rh9_gcc32/gtsrcid.exe \
  srcCatName="../../data/3EG.fits" \
  srcCatQty="3EG,RAJ2000,DEJ2000,theta95" \
  srcPosError="0.0" \
  cptCatName="../../data/radio_white1.4GHz.tsv" \
  cptCatQty="WB,_RAJ2000,_DEJ2000,S1.4,S4.85,S.365,Sp+Index,Sp+Index2" \
  cptPosError="0.0138888" \
  outCatName="../../data/outref/${RUN_ID}.fits" \
  probMethod="POSITION" \
  probThres="0.01" \
  maxNumCtp="100" \
  chatter="4" \
  clobber="yes" \
  debug="no" \
  mode="q" >& /dev/null
mv gtsrcid.log "../../data/outref/$RUN_ID.log"
