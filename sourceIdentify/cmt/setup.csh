# echo "Setting sourceIdentify v1r3p1 in /home/glast/dev"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /opt/projects/glast/tools/CMT/v1r16p20040701
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt build temporary_name -quiet`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt -quiet setup -csh -pack=sourceIdentify -version=v1r3p1 -path=/home/glast/dev  $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

