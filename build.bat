@echo off


::
set ENGINE_NAME=masEngine
set ENGINE_PROJ_PATH=%~dp0

:: Compiler Output Files' Directories
set BUILD_DIR=%ENGINE_PROJ_PATH%Build
set OBJ_DIR=%BUILD_DIR%\obj
set OBJ_PDB_DIR=%OBJ_DIR%\pdb
set ILK_DIR=%BUILD_DIR%
set EXE_PDB_DIR=%BUILD_DIR%
set MAP_DIR=%BUILD_DIR%
set EXE_DIR=%BUILD_DIR%


:: Create Dirctories if not existed
if not exist %BUILD_DIR%   ( mkdir %BUILD_DIR%   )
if not exist %OBJ_DIR%     ( mkdir %OBJ_DIR%     )
if not exist %OBJ_PDB_DIR% ( mkdir %OBJ_PDB_DIR% )
if not exist %ILK_DIR%     ( mkdir %ILK_DIR%     )
if not exist %EXE_PDB_DIR% ( mkdir %EXE_PDB_DIR% )
if not exist %MAP_DIR%     ( mkdir %MAP_DIR%     )
if not exist %EXE_DIR%     ( mkdir %EXE_DIR%     )


:: Compiler Output Files' setup
set OBJ_OUTPUT_DIR=-Fo"%OBJ_DIR%\\"
set OBJ_PDB_OUTPUT_DIR=-Fd"%OBJ_PDB_DIR%\\"
set ILK_OUTPUT_DIR=-ILK:"%ILK_DIR%\%ENGINE_NAME%.ilk"
set PDB_OUTPUT_DIR=-PDB:"%EXE_PDB_DIR%\%ENGINE_NAME%.pdb"
set MAP_OUTPUT_DIR=-MAP:"%MAP_DIR%\%ENGINE_NAME%.map"
set EXE_OUTPUT_DIR=-OUT:"%EXE_DIR%\%ENGINE_NAME%.exe"


::
set ENGINE_INC=-I "%ENGINE_PROJ_PATH%Inc\\"
set ENGINE_SRC=%ENGINE_PROJ_PATH%Src\*.cpp
set ENGINE_LIB=-LIBPATH:"%ENGINE_PROJ_PATH%Libs\"
set ENGINE_SYS_LIB=user32.lib Shlwapi.lib


:: Setup build command
set BuildEngine=cl
set BuildEngine=%BuildEngine% %CompileOptions%      &:: Pass compile flags
set BuildEngine=%BuildEngine% %OBJ_OUTPUT_DIR%      &:: Obj file output path
set BuildEngine=%BuildEngine% %OBJ_PDB_OUTPUT_DIR%  &:: Pdb file output path
set BuildEngine=%BuildEngine% %ENGINE_INC%          &:: Add include paths
set BuildEngine=%BuildEngine% %ENGINE_SRC%          &:: Add Source paths
set BuildEngine=%BuildEngine% -link                 &:: To pass linker options
set BuildEngine=%BuildEngine% %LinkOptions%         &:: Pass linker flags
set BuildEngine=%BuildEngine% %ENGINE_SYS_LIB%      &:: Add Libraries
set BuildEngine=%BuildEngine% %ENGINE_LIB%          &:: Add third party used libs
set BuildEngine=%BuildEngine% %ILK_OUTPUT_DIR%      &:: TODO
set BuildEngine=%BuildEngine% %PDB_OUTPUT_DIR%      &:: TODO
set BuildEngine=%BuildEngine% %MAP_OUTPUT_DIR%      &:: TODO
set BuildEngine=%BuildEngine% %EXE_OUTPUT_DIR%      &:: Output exe with app_name value


:: check that command prompt already setup with msvc's required paths to compile
where cl >nul 2>nul
if %errorlevel% neq 0 (
    call vcvarsall.bat x64
)


:: Call build command to compile project
echo Building %ENGINE_NAME% ...
%BuildEngine%
