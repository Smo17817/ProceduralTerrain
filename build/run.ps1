Set-Location -Path $PSScriptRoot

cmake -G "MinGW Makefiles" ..
mingw32-make
.\ProceduralTerrain.exe