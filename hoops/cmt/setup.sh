# echo "Setting hoops v0 in /u1/jchiang/SciTools/dev"

CMTROOT=/afs/slac.stanford.edu/g/glast/applications/CMT/v1r12p20021129; export CMTROOT
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt -quiet setup -sh -pack=hoops -version=v0 -path=/u1/jchiang/SciTools/dev $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

