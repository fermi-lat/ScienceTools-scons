# echo "Setting sourceIdentify sourceIdentify-02-02-04 in /home/glast/dev"

if test "${CMTROOT}" = ""; then
  CMTROOT=/opt/projects/glast/tools/CMT/v1r16p20040701; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet setup -sh -pack=sourceIdentify -version=sourceIdentify-02-02-04 -path=/home/glast/dev  $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

