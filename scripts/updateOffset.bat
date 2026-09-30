@echo off
REM =====================================================
REM Pixel Gun 3D Trainer — Offset Auto-Updater
REM Runs validate_offsets.py to update offsets.h from dump.cs
REM =====================================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "TOOLS_DIR=%PROJECT_ROOT%\resources\tools"
set "DUMPED_DIR=%PROJECT_ROOT%\resources\dumped\"
set "VALIDATOR=%TOOLS_DIR%\validate_offsets.py"
set "LOG_DIR=%PROJECT_ROOT%\resources\logs"

set "LOG_FILE=%LOG_DIR%\validate.log"

title PG3D Offset Updater

echo.
echo   ╔═════════════════════════════════════════════╗
echo   ║       PIXEL GUN 3D — Offset Updater         ║
echo   ╚═════════════════════════════════════════════╝
echo.

if "%~1"=="" (
    set "UPDATER_ARGS="
    goto Validate
)
if /i "%~1"=="validate" (
	set "UPDATER_ARGS="
    goto Validate
)
if /i "%~1"=="update" goto Update
if /i "%~1"=="help" goto Help

echo   [!] Unknown command: %1
echo   Run "%~nx0 help" for usage.
exit /b 1

REM =====================================================
REM Pre-flight
REM =====================================================
:Preflight
where python >nul 2>&1
if %errorlevel% neq 0 (
    where python3 >nul 2>&1
    if %errorlevel% neq 0 (
        echo   [!] Python is required but not found.
        echo       Download from https://www.python.org/downloads/
        exit /b 1
    )
    set "PYTHON=python3"
) else (
    set "PYTHON=python"
)

if not exist "%VALIDATOR%" (
    echo   [!] Validator script not found at: %VALIDATOR%
    exit /b 1
)

if not exist "%DUMPED_DIR%\dump.cs" (
    echo   [!] dump.cs not found at: %DUMPED_DIR%\dump.cs
    echo.
    echo   [!] Please place your IL2CPP dump files in:
    echo       %DUMPED_DIR%\
    echo.
    echo   Required files:
    echo     dump.cs          — IL2CPP class dump
    echo     il2cpp.h         — IL2CPP header (optional)
    echo     script.json      — IL2CPP script dump (optional)
    exit /b 1
)

echo   [+] Pre-flight checks passed
exit /b 0

REM =====================================================
REM Validate
REM =====================================================
:Validate
call :Preflight
if %errorlevel% neq 0 exit /b 1

echo   [*] Running offset auto-updater...
echo.

REM Create log directory
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM Run updater and log
%PYTHON% "%VALIDATOR%" %UPDATER_ARGS% > "%LOG_FILE%" 2>&1
type "%LOG_FILE%"

echo.

REM Check for results
findstr /C:"[UPDATED]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% equ 0 (
    echo   [+] Offsets were successfully updated! Review the output above.
)
findstr /C:"[WARNING]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% equ 0 (
    echo   [!] Some offsets were not found. Review the output above.
    echo.
    echo   [!] You may need to check the obfuscated field names in:
    echo       %PROJECT_ROOT%\src\utils\offsets.h
    echo.
)
findstr /C:"[UPDATED]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% neq 0 (
    findstr /C:"[WARNING]" "%LOG_FILE%" >nul 2>&1
    if %errorlevel% neq 0 (
        echo   [+] All offsets are already up-to-date!
    )
)

echo   [+] Log saved to: %LOG_FILE%
exit /b 0

REM =====================================================
REM Update
REM =====================================================
:Update
call :Preflight
if %errorlevel% neq 0 exit /b 1

echo   [*] Updating offsets using current dumps...
echo.
set "UPDATER_ARGS=--update"
goto Validate

REM =====================================================
REM Help
REM =====================================================
:Help
echo   Usage: %~nx0 ^<command^> [args]
echo.
echo   Commands:
echo     validate         — Run auto-updater against dump.cs
echo     update           — Update offsets using current dumps (--update)
echo     help             — Show this help message
echo.
echo   Examples:
echo     %~nx0 validate
echo     %~nx0 update
echo.
exit /b 0
