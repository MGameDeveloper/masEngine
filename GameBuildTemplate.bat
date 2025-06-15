@echo off

::
set GAME_NAME=masGame
set GAME_PROJ_PATH=%~dp0


:: Compiler Output Files' Directories
set BUILD_DIR=%GAME_PROJ_PATH%Build
set OBJ_DIR=%BUILD_DIR%\obj
set OBJ_PDB_DIR=%OBJ_DIR%\pdb
set ILK_DIR=%BUILD_DIR%
set DLL_PDB_DIR=%BUILD_DIR%
set MAP_DIR=%BUILD_DIR%
set DLL_DIR=%BUILD_DIR%


:: Create Dirctories if not existed
if not exist %BUILD_DIR%   ( mkdir %BUILD_DIR%   )
if not exist %OBJ_DIR%     ( mkdir %OBJ_DIR%     )
if not exist %OBJ_PDB_DIR% ( mkdir %OBJ_PDB_DIR% )
if not exist %ILK_DIR%     ( mkdir %ILK_DIR%     )
if not exist %DLL_PDB_DIR% ( mkdir %DLL_PDB_DIR% )
if not exist %MAP_DIR%     ( mkdir %MAP_DIR%     )
if not exist %DLL_DIR%     ( mkdir %DLL_DIR%     )


:: Compiler Output Files' setup
set OBJ_OUTPUT_DIR=-Fo"%OBJ_DIR%\\"
set OBJ_PDB_OUTPUT_DIR=-Fd"%OBJ_PDB_DIR%\\"
set ILK_OUTPUT_DIR=-ILK:"%ILK_DIR%\%GAME_NAME%.ilk"
set PDB_OUTPUT_DIR=-PDB:"%DLL_PDB_DIR%\%GAME_NAME%.pdb"
set MAP_OUTPUT_DIR=-MAP:"%MAP_DIR%\%GAME_NAME%.map"
set DLL_OUTPUT_DIR=-OUT:"%DLL_DIR%\%GAME_NAME%.dll"


::
set GAME_INC=-I"%GAME_PROJ_PATH%Inc\\"
set GAME_SRC=%GAME_PROJ_PATH%Src\*.cpp
set GAME_LIB=-LIBPATH:"%GAME_PROJ_PATH%Libs\"
set GAME_SYS_LIB=


:: Setup build command
set BuildGame=cl
set BuildGame=%BuildGame% %CompileOptions%      &:: Pass compile flags
set BuildGame=%BuildGame% %OBJ_OUTPUT_DIR%      &:: Obj file output path
set BuildGame=%BuildGame% %OBJ_PDB_OUTPUT_DIR%  &:: Pdb file output path
set BuildGame=%BuildGame% %GAME_INC%            &:: Add include paths
set BuildGame=%BuildGame% %GAME_SRC%            &:: Add Source paths
set BuildGame=%BuildGame% -link                 &:: To pass linker options
set BuildGame=%BuildGame% %LinkOptions%         &:: Pass linker flags
set BuildGame=%BuildGame% %GAME_SYS_LIB%        &:: Add Libraries
set BuildGame=%BuildGame% %GAME_LIB%            &:: Add third party used libs
set BuildGame=%BuildGame% %ILK_OUTPUT_DIR%      &:: TODO
set BuildGame=%BuildGame% %PDB_OUTPUT_DIR%      &:: TODO
set BuildGame=%BuildGame% %MAP_OUTPUT_DIR%      &:: TODO
set BuildGame=%BuildGame% %DLL_OUTPUT_DIR%      &:: Output exe with app_name value


echo.
echo Building %GAME_NAME% ...
echo %BUILD_DIR%
echo "%~dp0"
echo.

echo.
echo %BuildCmd%
echo.

:: Call build command to compile project
%BuildGame%