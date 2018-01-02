@echo off
set BLD=_build
IF NOT EXIST %~dp0\CMakeLists.txt GOTO :NOTSRC
IF NOT EXIST %~dp0\%BLD% mkdir %BLD%
pushd _build
cmake -G "Visual Studio 11" ^
    -DBGI_ENABLE_CT=YES ^
    -DBOOST_ROOT=D:\dev\boost\_svn\trunk ^
    ..
popd
GOTO :EOF

:NOTSRC
echo cannot find sources in '%~dp0'
