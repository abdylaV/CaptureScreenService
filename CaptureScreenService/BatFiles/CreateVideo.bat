@echo off
setlocal

set FFMPEG="D:\111\ffmpeg - 20160622 - e0faad8 - win64 - static\bin\ffmpeg.exe"

set INPUT_DIR="E:\Private\Shara\_VIDEO_\_test_\user\"
set INPUT_FOLDER="2016_7_2"

set FILES="\img_%%06d.png"

set OUTPUT_DIR=%INPUT_DIR:~1,-1%%INPUT_FOLDER:~1,-1%
set INPUT_FILES=%INPUT_DIR:~1,-1%%INPUT_FOLDER:~1,-1%%FILES:~1,-1%

rem echo %INPUT_FILES%
rem echo %OUTPUT_DIR%.mp4

FFMPEG -r 5 -i %INPUT_FILES% -vcodec libx264 %OUTPUT_DIR%.mp4

pause