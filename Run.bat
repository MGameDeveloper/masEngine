@echo off


:: check that command prompt already setup with msvc's required paths to compile
where cl >nul 2>nul
if %errorlevel% neq 0 (
    call vcvarsall.bat x64
)

::
"Build\masEngine.exe"