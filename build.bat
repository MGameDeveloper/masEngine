@echo off


::
call BuildVars.bat


::
set ENGINE_PATH=%~dp0


::
set ENGINE_NAME=masEngine


:: Compiler Output Files' Directories
set BUILD_DIR=%ENGINE_PATH%Build
set OBJ_DIR=%BUILD_DIR%\obj
set PDB_DIR=%BUILD_DIR%\pdb



:: Create Dirctories if not existed
if not exist %BUILD_DIR% ( mkdir %BUILD_DIR% )
if not exist %OBJ_DIR%   ( mkdir %OBJ_DIR%   )
if not exist %PDB_DIR%   ( mkdir %PDB_DIR%   )


::
set INC_DIR=-I"%ENGINE_PATH%Inc\\" -I"%ENGINE_PATH%Inc\GameAPI\\"
set SRC_DIR=%ENGINE_PATH%Src\*.cpp
set LIB_DIR=-LIBPATH:"%ENGINE_PATH%Lib\\"
set SYS_LIB=user32.lib Shlwapi.lib


:: Setup build command
set BuildEngine=cl
set BuildEngine=%BuildEngine% %CompileOptions%                      &:: Pass compile flags
set BuildEngine=%BuildEngine% -Fo:%OBJ_DIR%\                        &:: Obj file output path
set BuildEngine=%BuildEngine% -Fd:%PDB_DIR%\                        &:: Pdb file output path
set BuildEngine=%BuildEngine% %INC_DIR%                             &:: Add include paths
set BuildEngine=%BuildEngine% %SRC_DIR%                             &:: Add Source paths
set BuildEngine=%BuildEngine% -link                                 &:: To pass linker options
set BuildEngine=%BuildEngine% %LinkOptions%                         &:: Pass linker flags
set BuildEngine=%BuildEngine% %SYS_LIB%                             &:: Add Libraries
set BuildEngine=%BuildEngine% %LIB_DIR%                             &:: Add third party used libs
set BuildEngine=%BuildEngine% -ILK:"%BUILD_DIR%\%ENGINE_NAME%.ilk"  &:: TODO
set BuildEngine=%BuildEngine% -PDB:"%PDB_DIR%\%ENGINE_NAME%.pdb"    &:: TODO
set BuildEngine=%BuildEngine% -MAP:"%BUILD_DIR%\%ENGINE_NAME%.map"  &:: TODO
set BuildEngine=%BuildEngine% -OUT:"%BUILD_DIR%\%ENGINE_NAME%.exe"  &:: Output exe with app_name value


:: check that command prompt already setup with msvc's required paths to compile
where cl >nul 2>nul
if %errorlevel% neq 0 (
    call vcvarsall.bat x64
)


:: Call build command to compile project
echo Building %ENGINE_NAME% ...
%BuildEngine%
