rem Setting GRB v0 in d:\users\srobinsn\code-current
@echo off
set CMTROOT=d:\ground\CMT\v1r10p20011126
call %CMTROOT%\mgr\setup.bat

set tempfile=%HOMEDRIVE%%HOMEPATH%tmpsetup.bat
%CMTROOT%\%CMTBIN%\cmt.exe -quiet setup -bat -pack=GRB -version=v0 -path=d:\users\srobinsn\code-current %1 %2 %3 %4 %5 %6 %7 %8 %9 >%tempfile%
if exist %tempfile% call %tempfile%
if exist %tempfile% del %tempfile%
