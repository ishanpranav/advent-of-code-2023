@Echo Off

..\day01a.exe < ..\data\011.txt
..\day01b.exe < ..\data\012.txt

For /l %%i In (2,1,7) Do (
    ..\day0%%ia.exe < ..\data\0%%i1.txt
    ..\day0%%ib.exe < ..\data\0%%i1.txt
)

..\day08a.exe < ..\data\081.txt
..\day08a.exe < ..\data\082.txt
..\day08b.exe < ..\data\083.txt

For /l %%i In (9,1,9) Do (
    ..\day0%%ia.exe < ..\data\0%%i1.txt
    ..\day0%%ib.exe < ..\data\0%%i1.txt
)

..\day10a.exe < ..\data\101.txt
..\day10a.exe < ..\data\102.txt
..\day11a.exe < ..\data\111.txt

For /l %%i In (12,1,16) Do (
    ..\day%%ia.exe < ..\data\%%i1.txt
    ..\day%%ib.exe < ..\data\%%i1.txt
)

..\day17a.exe < ..\data\171.txt
..\day17b.exe < ..\data\171.txt
..\day17b.exe < ..\data\172.txt

For /l %%i In (18,1,19) Do (
    ..\day%%ia.exe < ..\data\%%i1.txt
    ..\day%%ib.exe < ..\data\%%i1.txt
)

..\day20a.exe < ..\data\201.txt
..\day20a.exe < ..\data\202.txt

For /l %%i In (22,1,22) Do (
    ..\day%%ia.exe < ..\data\%%i1.txt
    ..\day%%ib.exe < ..\data\%%i1.txt
)

..\day24b.exe < ..\data\241.txt
..\day25z.exe < ..\data\251.txt
