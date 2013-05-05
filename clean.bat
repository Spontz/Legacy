@echo off
echo Borrando basura ...
del /s /f *.DS_Store
del /s /f ._*
del /s /f Thumbs.db
pause
echo.
echo Borrando archivos de compilacion ...
del /s /f /q build
del /s /f /q Debug
del /s /f /q Release
del /s /f /q *svn*
rmdir /s /q build
rmdir /s /q Debug
rmdir /s /q Release
rmdir /s /q .svn
pause
