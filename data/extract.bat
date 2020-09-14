echo off
setlocal EnableExtensions EnableDelayedExpansion
for /R %%i in (*.tif) do (
    set "PathTif=%%i"
    set "PathIso=!PathTif:tif=iso!"
    python extract.py "!PathTif!" "!PathIso!"
)
endlocal
pause
