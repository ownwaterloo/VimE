cl /DLIB plus.c
dumpbin /exports plus.exe
dumpbin /exports plus.lib
cl /Feclient.exe plus.c plus.lib
