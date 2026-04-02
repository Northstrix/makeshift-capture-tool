@echo off
setlocal
echo.
echo  ================================================
echo    Makeshift Capture Tool  --  x64 Build
echo  ================================================
echo.

set "VCPKG_ROOT=C:\mcp\vcpkg"
set "TRIPLET=x64-windows-static"
set "BOOST_INC=%VCPKG_ROOT%\installed\%TRIPLET%\include"
set "BOOST_LIB=%VCPKG_ROOT%\installed\%TRIPLET%\lib"

if not exist "%BOOST_INC%\boost\filesystem.hpp" (
    echo [ERROR] Boost not found at %BOOST_INC%
    pause & exit /b 1
)
if not exist "nuklear.h" (
    echo [ERROR] nuklear.h not found.
    pause & exit /b 1
)
if not exist "makeshift_capture.cpp" (
    echo [ERROR] makeshift_capture.cpp not found.
    pause & exit /b 1
)
if not exist "logo.png" (
    echo [WARN ] logo.png not found – app will use built-in icon.
    echo         Place logo.png next to this .bat to embed it.
    echo.
)
if not exist "%BOOST_LIB%\boost_filesystem-vc145-mt-x64-1_90.lib" (
    echo [ERROR] boost_filesystem-vc145-mt-x64-1_90.lib not found.
    dir "%BOOST_LIB%\*.lib" /b
    pause & exit /b 1
)

echo [OK] Boost  : %BOOST_INC%
echo [OK] Target : x64
echo.

:: Compile logo.png into a .res file (requires logo.png in same folder)
if exist "logo.png" (
    echo [..] Embedding logo.png into resource.res ...
    rc.exe /nologo resource.rc
    if %ERRORLEVEL% neq 0 (
        echo [WARN] rc.exe failed - building without embedded logo.
        set "RESFILE="
    ) else (
        set "RESFILE=resource.res"
        echo [OK] logo.png embedded.
    )
) else (
    set "RESFILE="
)

echo [..] Compiling x64 ...
echo.

cl.exe /nologo /std:c++17 /EHsc /O2 /W1 /MT /GL /DNDEBUG ^
    /D_WIN32_WINNT=0x0601 ^
    /DBOOST_ALL_NO_LIB ^
    /I"%BOOST_INC%" /I. ^
    makeshift_capture.cpp ^
    /Fe:MakeshiftCapture_x64.exe ^
    /link /NOLOGO /LTCG /OPT:REF /OPT:ICF ^
    /SUBSYSTEM:WINDOWS,6.1 ^
    /MACHINE:X64 ^
    /LIBPATH:"%BOOST_LIB%" ^
    boost_filesystem-vc145-mt-x64-1_90.lib ^
    boost_chrono-vc145-mt-x64-1_90.lib ^
    boost_atomic-vc145-mt-x64-1_90.lib ^
    boost_date_time-vc145-mt-x64-1_90.lib ^
    kernel32.lib user32.lib gdi32.lib shell32.lib ^
    comctl32.lib ole32.lib advapi32.lib gdiplus.lib ^
    %RESFILE%

if %ERRORLEVEL% neq 0 (
    echo.
    echo [FAILED] See errors above.
    pause & exit /b 1
)

echo.
echo ================================================================
echo  BUILD SUCCESSFUL: MakeshiftCapture_x64.exe
echo ================================================================
echo.
echo  No additional DLLs needed. The exe is fully self-contained.
echo  logo.png is embedded in the exe if it was present during build.
echo.
pause
