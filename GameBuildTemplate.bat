@echo off

call "%~dp0../../Engine/BuildVars.bat"


::
set GAME_NAME=masGame

:: Compiler Output Files' Directories
set BUILD_DIR="%~dp0Build"
set OBJ_DIR="%BUILD_DIR%\obj"


:: Create Dirctories if not existed
if not exist %BUILD_DIR%( mkdir %BUILD_DIR%   )
if not exist %OBJ_DIR%  ( mkdir %OBJ_DIR%     )


::
set GAME_INC=-I"%~dp0Inc\\"
set GAME_SRC=%~dp0Src\*.cpp
set GAME_LIB=-LIBPATH:"%~dp0Libs\\"
set GAME_SYS_LIB=


:: Setup build command
set BuildGame=cl
set BuildGame=%BuildGame% %CompileOptions%                   &:: Pass compile flags
set BuildGame=%BuildGame% -Fo %OBJ_DIR%                      &:: Obj file output path
set BuildGame=%BuildGame% -Fd %OBJ_PDB_DIR%                  &:: Pdb file output path
set BuildGame=%BuildGame% %GAME_INC%                         &:: Add include paths
set BuildGame=%BuildGame% %GAME_SRC%                         &:: Add Source paths
set BuildGame=%BuildGame% -link                              &:: To pass linker options
set BuildGame=%BuildGame% %LinkOptions%                      &:: Pass linker flags
set BuildGame=%BuildGame% %GAME_SYS_LIB%                     &:: Add Libraries
set BuildGame=%BuildGame% %GAME_LIB%                         &:: Add third party used libs
set BuildGame=%BuildGame% -ILK:"%BUILD_DIR%\%GAME_NAME%.ilk" &:: TODO
set BuildGame=%BuildGame% -PDB:"%BUILD_DIR%\%GAME_NAME%.pdb" &:: TODO
set BuildGame=%BuildGame% -MAP:"%BUILD_DIR%\%GAME_NAME%.map" &:: TODO
set BuildGame=%BuildGame% -OUT:"%BUILD_DIR%\%GAME_NAME%.dll" &:: Output exe with app_name value


:: Call build command to compile project
%BuildGame%