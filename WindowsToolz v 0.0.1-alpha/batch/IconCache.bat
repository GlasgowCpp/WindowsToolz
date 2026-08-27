taskkill /f /im explorer.exe
del %LocalAppdata%\IconCache.db
start explorer.exe
@echo off
pause
exit