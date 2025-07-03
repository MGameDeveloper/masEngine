@echo off


::
call "%~dp0..\..\Engine\BuildVars.bat"


::
set GAME_PATH=%~dp0


::
set GAME_NAME=masGame


:: Compiler Output Files' Directories
set BUILD_FOLDER=%1
set BUILD_DIR=%GAME_PATH%%BUILD_FOLDER%
set OBJ_DIR=%BUILD_DIR%\obj
set PDB_DIR=%BUILD_DIR%\pdb


:: Create Dirctories if not existed
if not exist %BUILD_DIR% ( mkdir %BUILD_DIR% )
if not exist %OBJ_DIR%   ( mkdir %OBJ_DIR%   )
if not exist %PDB_DIR%   ( mkdir %PDB_DIR%   )


::
set INC_DIR=-I"%GAME_PATH%Inc\\" -I"%GAME_PATH%\..\..\Inc\GameAPI\\"
set SRC_DIR=%GAME_PATH%Src\*.cpp
set LIB_DIR=-LIBPATH:"%GAME_PATH%Lib\\" -LIBPATH:"%GAME_PATH%\..\..\Build\"
set SYS_LIB=masEngine.lib DXGI.lib D3D11.lib D3DCompiler.lib

:: Setup build command
set BuildGame=cl
set BuildGame=%BuildGame% -LD -DMAS_GAME_EXPORT_API %CompileOptions%  &:: Pass compile flags
set BuildGame=%BuildGame% -Fo%OBJ_DIR%\                        &:: Obj file output path
set BuildGame=%BuildGame% -Fd%PDB_DIR%\                        &:: Pdb file output path
set BuildGame=%BuildGame% %INC_DIR%                            &:: Add include paths
set BuildGame=%BuildGame% %SRC_DIR%                            &:: Add Source paths
set BuildGame=%BuildGame% -link                                &:: To pass linker options
set BuildGame=%BuildGame% -DLL %LinkOptions%                   &:: Pass linker flags
set BuildGame=%BuildGame% %SYS_LIB%                            &:: Add Libraries
set BuildGame=%BuildGame% %LIB_DIR%                            &:: Add third party used libs
set BuildGame=%BuildGame% -IMPLIB:"%BUILD_DIR%\%GAME_NAME%.lib" &:: 
::set BuildGame=%BuildGame% -NOEXP                              &:: 
set BuildGame=%BuildGame% -ILK:"%BUILD_DIR%\%GAME_NAME%.ilk"   &:: TODO
set BuildGame=%BuildGame% -PDB:"%PDB_DIR%\%GAME_NAME%.pdb"     &:: TODO
set BuildGame=%BuildGame% -MAP:"%BUILD_DIR%\%GAME_NAME%.map"   &:: TODO
set BuildGame=%BuildGame% -OUT:"%BUILD_DIR%\%GAME_NAME%.dll"   &:: Output exe with app_name value


:: Call build command to compile project
echo Building %GAME_NAME% ...
%BuildGame%

