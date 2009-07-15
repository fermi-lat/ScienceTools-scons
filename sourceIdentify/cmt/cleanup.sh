if test "${CMTROOT}" = ""; then
  CMTROOT=/opt/projects/glast/tools/CMT/v1r16p20040701; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet cleanup -sh -pack=sourceIdentify -version=v2r2p1 -path=/home2/glast/dev $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

