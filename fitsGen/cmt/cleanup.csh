set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet cleanup -csh -pack=fitsGen -version=v0 -path=/u1/jchiang/SciTools/dev $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

