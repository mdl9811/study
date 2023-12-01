@REM Copyright (c) 2023 mdl. All rights reserved.

@echo off
setlocal

set LLVM=C:\Program Files\LLVM
set BUILD=debug
set TARGET=windows

if "%1" == "--release" set BUILD=release
if "%1" == "--debug" set BUILD=debug

echo build %BUILD%

set "CLANG=%LLVM%\bin\clang.exe"
set "LLD_LINK=%LLVM%\bin\lld-link.exe"
set "AR=%LLVM%\bin\llvm-ar.exe"

if not exist out mkdir out

(
  echo BUILD=%BUILD%
  echo TARGET=%TARGET%
  echo CLANG="%CLANG%"
  echo AR="%AR%"
  echo LLD_LINK="%LLD_LINK%"
) > out/args.ninja
