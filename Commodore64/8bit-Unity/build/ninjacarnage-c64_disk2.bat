echo off

setlocal enableextensions enabledelayedexpansion

mkdir c64
cd ..

del build\c64\*.* /F /Q

echo --------------- CONVERT ASSETS ---------------  

utils\py27\python utils\scripts\c64\C64Sprites.py D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/sprites/SpritesCursors.png build/c64/sprites.dat
utils\py27\python utils\scripts\ProcessChunks.py c64 D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/chunks/chunks-c64.txt build/c64/

utils\scripts\c64\sidreloc.exe -v -z 30-ff -p 08 D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/speednik-c64.sid build/c64/speednik.sid
if exist build/c64/speednik.sid (
    utils\scripts\c64\psid64.exe -n build/c64/speednik.sid
) else (
    echo Relocation impossible, using the original file instead...
    copy D:\Kristof\C64_Dev\8bit-Unity\projects\ninja\music\speednik-c64.sid build\c64\speednik.prg
)

echo DONE!

echo --------------- COMPILE PROGRAM ---------------

utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\bitmap.c
utils\cc65\bin\ca65 unity\bitmap.s
del unity\bitmap.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\chunks.c
utils\cc65\bin\ca65 unity\chunks.s
del unity\chunks.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\geom2d.c
utils\cc65\bin\ca65 unity\geom2d.s
del unity\geom2d.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\network.c
utils\cc65\bin\ca65 unity\network.s
del unity\network.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\print.c
utils\cc65\bin\ca65 unity\print.s
del unity\print.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\sfx.c
utils\cc65\bin\ca65 unity\sfx.s
del unity\sfx.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\sprites.c
utils\cc65\bin\ca65 unity\sprites.s
del unity\sprites.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\widgets.c
utils\cc65\bin\ca65 unity\widgets.s
del unity\widgets.s
utils\cc65\bin\cc65 -Cl -O -t c64 -I unity unity\C64\display.c
utils\cc65\bin\ca65 unity\C64\display.s
del unity\C64\display.s
utils\cc65\bin\ca65 unity\chars.s
utils\cc65\bin\ca65 unity\math.s
utils\cc65\bin\ca65 unity\C64\JOY.s
utils\cc65\bin\ca65 unity\C64\ROM.s
utils\cc65\bin\ca65 unity\C64\SID.s
utils\cc65\bin\ar65 r build/c64/unity.lib unity\bitmap.o unity\chunks.o unity\geom2d.o unity\network.o unity\print.o unity\sfx.o unity\sprites.o unity\widgets.o unity\C64\display.o unity\chars.o unity\math.o unity\C64\JOY.o unity\C64\ROM.o unity\C64\SID.o 
utils\cc65\bin\cl65 -o build/c64/ninjacarnage.bin -m build/c64/map64.txt -l build/c64/ninja.s -Cl -O -t c64 -C unity/C64/c64.cfg -I unity D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/src/ninja.c D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/src/ninja_additionnal.s build/c64/unity.lib unity/IP65/ip65.lib unity/IP65/ip65_c64.lib

utils\scripts\exomizer.exe sfx $180d build/c64/ninjacarnage.bin build/c64/sprites.dat -o build/c64/loader.prg

echo DONE!

echo --------------- C64 DISK BUILDER --------------- 

set C1541=utils\scripts\c64\c1541 -format "ninja carnage",666 d64 build/ninjacarnage-disk2-c64.d64 -attach build/ninjacarnage-disk2-c64.d64 -write build/c64/loader.prg loader.prg -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL61.BIN level61.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/TIMEUP.BIN timeup.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL62.BIN level62.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL63.BIN level63.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL71.BIN level71.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL72.BIN level72.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL73.BIN level73.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL81.BIN level81.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL82.BIN level82.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL83.BIN level83.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL91.BIN level91.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL92.BIN level92.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL93.BIN level93.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVELC1.BIN levelc1.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVELC2.BIN levelc2.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVELC3.BIN levelc3.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC6N.BIN music6n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC7N.BIN music7n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC8N.BIN music8n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC9N.BIN music9n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSICCN.BIN musiccn.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSICCO.BIN musicco.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/GAME.BIN game.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/SIDE2.BIN side2.bin
for /f "tokens=*" %%A in (build\c64\chunks.lst) do set C1541=!C1541!-write %%A %%~nxA 
%C1541%


echo DONE
pause

cd "utils\emulators\WinVICE-2.4"
x64.exe "..\..\..\build\ninjacarnage-disk2-c64.d64"
