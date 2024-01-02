@Echo Off

For /l %%i In (1,1,9) Do (
    ..\day0%%ia.exe < ..\alt0%%i.txt
    ..\day0%%ib.exe < ..\alt0%%i.txt
)

..\day10a.exe < ../alt10.txt

For /l %%i In (11,1,20) Do (
    ..\day%%ia.exe < ..\alt%%i.txt
    ..\day%%ib.exe < ..\alt%%i.txt
)
