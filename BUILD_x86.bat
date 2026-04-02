@echo off
setlocal EnableExtensions EnableDelayedExpansion

echo.
echo  ================================================
echo    Makeshift Capture Tool  --  x86 (32-bit) Build
echo  ================================================
echo.
echo  Run from: "x86 Native Tools Command Prompt for VS 2022"
echo.

set "VCPKG_ROOT=C:\mct\vcpkg"
set "TRIPLET=x86-windows-static"
set "BOOST_INC=%VCPKG_ROOT%\installed\%TRIPLET%\include"
set "BOOST_LIB=%VCPKG_ROOT%\installed\%TRIPLET%\lib"

if not exist "%BOOST_INC%\boost\filesystem.hpp" (
    echo [ERROR] Boost headers not found at:
    echo   %BOOST_INC%
    echo.
    echo Check that Boost is actually installed under:
    echo   %VCPKG_ROOT%\installed\%TRIPLET%
    pause
    exit /b 1
)

if not exist "nuklear.h" (
    echo [ERROR] nuklear.h not found.
    pause
    exit /b 1
)

if not exist "makeshift_capture.cpp" (
    echo [ERROR] makeshift_capture.cpp not found.
    pause
    exit /b 1
)

echo [OK] Boost  : %BOOST_INC%
echo [OK] Target : x86
echo.

set "BOOST_FS_LIB="
for /f "delims=" %%F in ('dir /b "%BOOST_LIB%\boost_filesystem*.lib" 2^>nul') do set "BOOST_FS_LIB=%%F"
set "BOOST_CHRONO_LIB="
for /f "delims=" %%F in ('dir /b "%BOOST_LIB%\boost_chrono*.lib" 2^>nul') do set "BOOST_CHRONO_LIB=%%F"
set "BOOST_ATOMIC_LIB="
for /f "delims=" %%F in ('dir /b "%BOOST_LIB%\boost_atomic*.lib" 2^>nul') do set "BOOST_ATOMIC_LIB=%%F"
set "BOOST_DT_LIB="
for /f "delims=" %%F in ('dir /b "%BOOST_LIB%\boost_date_time*.lib" 2^>nul') do set "BOOST_DT_LIB=%%F"

if not defined BOOST_FS_LIB (
    echo [ERROR] Cannot find boost_filesystem*.lib in %BOOST_LIB%
    dir /b "%BOOST_LIB%\*filesystem*.lib" 2>nul
    pause
    exit /b 1
)

if not defined BOOST_CHRONO_LIB (
    echo [ERROR] Cannot find boost_chrono*.lib in %BOOST_LIB%
    pause
    exit /b 1
)

if not defined BOOST_ATOMIC_LIB (
    echo [ERROR] Cannot find boost_atomic*.lib in %BOOST_LIB%
    pause
    exit /b 1
)

if not defined BOOST_DT_LIB (
    echo [ERROR] Cannot find boost_date_time*.lib in %BOOST_LIB%
    pause
    exit /b 1
)

echo [OK] filesystem : !BOOST_FS_LIB!
echo [OK] chrono     : !BOOST_CHRONO_LIB!
echo [OK] atomic     : !BOOST_ATOMIC_LIB!
echo [OK] date_time  : !BOOST_DT_LIB!
echo.

set "RESFILE="

if exist "logo.png" if exist "resource.rc" (
    rc.exe /nologo resource.rc
    if !ERRORLEVEL! equ 0 set "RESFILE=resource.res"
)

echo [..] Compiling x86 ...
echo.

cl.exe /nologo /std:c++17 /EHsc /O2 /W1 /MT /GL /DNDEBUG ^
    /D_WIN32_WINNT=0x0601 ^
    /DBOOST_ALL_NO_LIB ^
    /I"%BOOST_INC%" /I. ^
    makeshift_capture.cpp ^
    /Fe:MakeshiftCapture_x86.exe ^
    /link /NOLOGO /LTCG /OPT:REF /OPT:ICF ^
    /SUBSYSTEM:WINDOWS,6.1 ^
    /MACHINE:X86 ^
    /LIBPATH:"%BOOST_LIB%" ^
    "!BOOST_FS_LIB!" ^
    "!BOOST_CHRONO_LIB!" ^
    "!BOOST_ATOMIC_LIB!" ^
    "!BOOST_DT_LIB!" ^
    kernel32.lib user32.lib gdi32.lib shell32.lib ^
    comctl32.lib ole32.lib advapi32.lib gdiplus.lib ^
    !RESFILE!

if errorlevel 1 (
    echo.
    echo [FAILED]
    pause
    exit /b 1
)

echo.
echo ================================================================
echo  BUILD SUCCESSFUL: MakeshiftCapture_x86.exe
echo ================================================================
echo.
pause