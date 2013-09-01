tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet cleanup -sh -pack=fitsGen -version=v0 -path=/u1/jchiang/SciTools/dev $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

