IF EXIST main.exe DEL main.exe
C:\raylib\w64devkit\bin\g++ -o main.exe main.cpp C:\raylib\raylib\src\raylib.rc.data -s -static -Os -Wall -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm
IF EXIST main.exe main.exe