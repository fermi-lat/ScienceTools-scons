# echo "Setting fitsGen v0 in /u1/jchiang/SciTools/dev"

setenv CMTROOT /afs/slac.stanford.edu/g/glast/applications/CMT/v1r12p20021129
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet setup -csh -pack=fitsGen -version=v0 -path=/u1/jchiang/SciTools/dev $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

