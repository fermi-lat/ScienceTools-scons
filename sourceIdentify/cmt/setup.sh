# echo "Setting sourceIdentify v1r0p1 in /home/knodlseder/dev/glast"

if test "${CMTROOT}" = ""; then
  CMTROOT=/opt/projects/glast/tools/CMT/v1r16p20040701; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet setup -sh -pack=sourceIdentify -version=v1r0p1 -path=/home/knodlseder/dev/glast  $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

