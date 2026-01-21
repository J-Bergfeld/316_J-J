@echo off
cd /d "%~dp0"

if "%~1"=="" (
    set /p MSG=Enter commit message: 
) else (
    set MSG=%~1
)

git add .
git commit -m "%MSG%"
git push

pause
