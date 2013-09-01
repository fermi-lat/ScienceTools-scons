#!/bin/tcsh -f
#
# Testrun 2
#
set    RUN_ID = "test2"
setenv PFILES ../../pfiles
#
# Find Cygnus region 3EG counterparts in IRAS IPAC catalogue
#===========================================================
gtsrcid.exe \
  srcCatName="../../data/cygob2_egret_3eg.tsv" \
  srcCatQty="3EG,RAJ2000,DEJ2000,theta95" \
  cptCatName="../../data/cygob2_iras_ipac_pt.tsv" \
  cptCatQty="" \
  outCatName="../../data/outref/${RUN_ID}.fits" \
  probMethod="POSITION" \
  probThres="0.60" \
  maxNumCtp="100" \
  chatter="4" \
  clobber="yes" \
  debug="no" \
  mode="ql" >& "../../data/outref/$RUN_ID.dump"
mv gtsrcid.log "../../data/outref/$RUN_ID.log"
