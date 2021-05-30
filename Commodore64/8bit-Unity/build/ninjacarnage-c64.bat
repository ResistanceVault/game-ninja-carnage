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

set C1541=utils\scripts\c64\c1541 -format "ninja carnage",666 d64 build/ninjacarnage-c64.d64 -attach build/ninjacarnage-c64.d64 -write build/c64/loader.prg loader.prg -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL11.BIN level11.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/TIMEUP.BIN timeup.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL12.BIN level12.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL13.BIN level13.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL01.BIN level01.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL02.BIN level02.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL21.BIN level21.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL22.BIN level22.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL23.BIN level23.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL31.BIN level31.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL32.BIN level32.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL33.BIN level33.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL41.BIN level41.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL42.BIN level42.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL43.BIN level43.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL51.BIN level51.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL52.BIN level52.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVEL53.BIN level53.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/LEVELD1.BIN leveld1.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSICIN.BIN musicin.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC0N.BIN music0n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC1N.BIN music1n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC1O.BIN music1o.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC2N.BIN music2n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC2O.BIN music2o.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC3N.BIN music3n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC3O.BIN music3o.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC4N.BIN music4n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSIC5N.BIN music5n.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/music/MUSICDN.BIN musicdn.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/GAME.BIN game.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/INTRO1.BIN intro1.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/INTRO2.BIN intro2.bin -write D:/Kristof/C64_Dev/8bit-Unity/projects/ninja/levels/INTRO3.BIN intro3.bin
for /f "tokens=*" %%A in (build\c64\chunks.lst) do set C1541=!C1541!-write %%A %%~nxA 
%C1541%


echo DONE
pause

cd "utils\emulators\WinVICE-2.4"
x64.exe "..\..\..\build\ninjacarnage-c64.d64"
