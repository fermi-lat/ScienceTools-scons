# echo "Setting pgwave v1 in /a/sulky36/g.glast.u33/tosti/myPgwave"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/slac/g/glast/applications/CMT/v1r18p20061003; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=pgwave -version=v1 -path=/a/sulky36/g.glast.u33/tosti/myPgwave  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

