@Echo Off
Set cc=Clang
Set cflags=-O3 -pedantic -std=c99 -Wall -Wextra -Wno-deprecated-declarations
Set twosComplement=-fwrapv

For /l %%i In (1,1,4) Do (
  Echo %cc% %cflags% ..\src\day0%%ia.c -o ..\day0%%ia.exe
  %cc% %cflags% ..\src\day0%%ia.c -o ..\day0%%ia.exe
  Echo %cc% %cflags% ..\src\day0%%ib.c -o ..\day0%%ib.exe
  %cc% %cflags% ..\src\day0%%ib.c -o ..\day0%%ib.exe
)

Echo %cc% %cflags% ..\src\day05a.c -o ..\day05a.exe
%cc% %cflags% ..\src\day05a.c -o ..\day05a.exe
Echo %cc% %cflags% %twosComplement% ..\src\days05b.c -o ..\day05b.c
%cc% %cflags% %twosComplement% ..\src\day05b.c -o ..\day05b.exe

For /l %%i In (6,1,9) Do (
  Echo %cc% %cflags% ..\src\day0%%ia.c -o ..\day0%%ia.exe
  %cc% %cflags% ..\src\day0%%ia.c -o ..\day0%%ia.exe
  Echo %cc% %cflags% ..\src\day0%%ib.c -o ..\day0%%ib.exe
  %cc% %cflags% ..\src\day0%%ib.c -o ..\day0%%ib.exe
)

Echo %cc% %cflags% ..\src\day10a.c -o ..\day10a.exe
%cc% %cflags% ..\src\day10a.c -o ..\day10a.exe

For /l %%i In (11,1,20) Do (
  Echo %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
  %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
  Echo %cc% %cflags% ..\src\day%%ib.c -o ..\day%%ib.exe
  %cc% %cflags% ..\src\day%%ib.c -o ..\day%%ib.exe
)

Echo %cc% %cflags% ..\src\day21a.c -o ..\day21a.exe
%cc% %cflags% ..\src\day21a.c -o ..\day21a.exe

For /l %%i In (22,1,22) Do (
  Echo %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
  %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
)

For /l %%i In (24,1,24) Do (
  Echo %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
  %cc% %cflags% ..\src\day%%ia.c -o ..\day%%ia.exe
)

Echo %cc% %cflags% ..\src\day25z.c -o ..\day25z.exe
%cc% %cflags% ..\src\day25z.c -o ..\day25z.exe
