@echo off
set CURR_DIR=%cd%
set WORKING_DIR=%CURR_DIR%\..\src\user_interface\kvm_client\dll_project

call:clean_all

cd %WORKING_DIR%\kvm_dll
call:compile cursor.sln

cd %WORKING_DIR%\vmm_dll
call:compile VMConsoleLib.sln

Exit

:clean_all
    cd %WORKING_DIR%\kvm_dll
    call:clean_proj

    cd %WORKING_DIR%\vmm_dll
    call:clean_proj
goto:eof

:clean_proj
    rmdir /S /Q BackUp*
    rmdir /S /Q Debug
    rmdir /S /Q Release
    rmdir /S /Q x64
goto:eof

:compile
    set sln_name=%1
    devenv %sln_name% /Clean
    devenv %sln_name% /Build "Release|x86"
    devenv %sln_name% /Clean
    devenv %sln_name% /Build "Release|x64"
goto:eof
