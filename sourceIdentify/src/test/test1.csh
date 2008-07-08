#!/bin/tcsh -f
#
# Testrun 1
#
set    RUN_ID = "test1"
setenv PFILES ../../pfiles
setenv PATH   .:$PATH

#
# Find 3EG counterparts in North 20 cm survey catalogue of White et al. 1992
# and create some fake flux probability ratio.
#
# Note the special syntax using the ' ' quotes and the $ $ enclosing. The
# ' ' quotes assure that the tcsh does not interpret the $ symbols. The
# $ $ enclosing is needed by cfitsio when interpreting the new quantity
# string to avoid a misinterpretation of the @, . or - symbols in the
# column name ...
#===========================================================================
gtsrcid \
  srcCatName="../../data/3EG.fits" \
  srcCatPrefix="3EG" \
  srcCatQty="3EG,RAJ2000,DEJ2000,theta95,F" \
  srcPosError="0.0" \
  cptCatName="../../data/radio_white1.4GHz.tsv" \
  cptCatPrefix="WB14" \
  cptCatQty="WB,_RAJ2000,_DEJ2000,S1.4,S4.85,S.365,Sp+Index,Sp+Index2" \
  cptPosError="0.0138888" \
  cptDensFile="" \
  outCatName="${RUN_ID}.fits" \
  outCatQty01='F-Ratio = $%3EG_F$ / $%WB14_S1.4$' \
  outCatQty02='PROB-Ratio = exp(-0.5*(( $F-Ratio$ + 0.1 )/0.1)^2)' \
  outCatQty03="" \
  outCatQty04="" \
  outCatQty05="" \
  outCatQty06="" \
  outCatQty07="" \
  outCatQty08="" \
  outCatQty09="" \
  probMethod="PROB_POST" \
  probPrior="0.01" \
  probThres="0.05" \
  maxNumCpt="4" \
  fom="" \
  select01='' \
  select02='' \
  select03='' \
  select04="" \
  select05="" \
  select06="" \
  select07="" \
  select08="" \
  select09="" \
  chatter="2" \
  clobber="yes" \
  debug="no" \
  mode="q" #>& /dev/null
mv gtsrcid.log "${RUN_ID}.log"
