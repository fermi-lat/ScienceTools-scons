# echo "Setting sourceIdentify v0r1p1 in /home/knodlseder/glast/src"

if test "${CMTROOT}" = ""; then
  CMTROOT=/opt/projects/glast/extlib/CMT/v1r16p20040701; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet setup -sh -pack=sourceIdentify -version=v0r1p1 -path=/home/knodlseder/glast/src  $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

