@echo off


::GENERAL COMPILE OPTIONS
set CompileOptions=                  -nologo                   &:: Suppress Startup Banner
set CompileOptions=%CompileOptions%  -Od                       &:: Disable optimization
set CompileOptions=%CompileOptions%  -Oi                       &:: Use assembly intrinsics where possible
set CompileOptions=%CompileOptions%  -MT                       &:: Include CRT library in the executable (static link)
set CompileOptions=%CompileOptions%  -Gm-                      &:: Disable minimal rebuild
set CompileOptions=%CompileOptions%  -GR-                      &:: Disable runtime type info (C++)
set CompileOptions=%CompileOptions%  -EHa-                     &:: Disable exception handling (C++)
set CompileOptions=%CompileOptions%  -WX                       &:: Treat all warnings as errors 
set CompileOptions=%CompileOptions%  -DDEBUG                   &:: Define DEBUG macro
set CompileOptions=%CompileOptions%  -D_DEBUG                  &:: Define _DEBUG macro
set CompileOptions=%CompileOptions%  -DUNICODE                 &:: Define UNICODe macro
set CompileOptions=%CompileOptions%  -D_UNICODE                &:: Define _UNICODE macro
set CompileOptions=%CompileOptions%  -Zi                       &:: Generate debug information
set CompileOptions=%CompileOptions%  -RTCc                     &:: Detects data loss when assigning values to samller types
set CompileOptions=%CompileOptions%  -RTC1                     &:: Check stack corruption, array overrun and use of uninitialized variables
set CompileOptions=%CompileOptions%  -fastfail                 &::
::set CompileOptions=%CompileOptions%  -analyze                  &::
set CompileOptions=%CompileOptions%  -FC                       &:: Displays the full path of source code files passed to cl.exe in diagnostic text
set CompileOptions=%CompileOptions%  -MP                       &:: Builds multiple source files concurrently
set CompileOptions=%CompileOptions%  -FS                       &:: 
::set CompileOptions=%CompileOptions%  -Wall                     &:: Enable all warnings, including warnings that are disabled by default
set CompileOptions=%CompileOptions%  -options:strict           &:: Unrecognized compiler options are errors.
set CompileOptions=%CompileOptions%  -diagnostics:caret        &:: Diagnostics format prints column and the indicated line of source
set CompileOptions=%CompileOptions%  -sdl                      &:: Enable more security and warnings
set CompileOptions=%CompileOptions%  -WL                       &:: Enable one-line diagnostics for error and warning messages when compiling c++ source code from the command line
set CompileOptions=%CompileOptions%  -D_CRT_SECURE_NO_WARNINGS &::


:: GENERAL LINKER OPTIONS
set LinkOptions=              -DEBUG       &:: Create debug information
set LinkOptions=%LinkOptions% -INCREMENTAL &:: 
set LinkOptions=%LinkOptions% -WX          &:: 
::set LinkOptions=%LinkOptions% -PROFILER    &::