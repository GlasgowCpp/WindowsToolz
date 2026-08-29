@echo off
sfc /scannow
DISM /Online /Cleanup-image /Restorehealth
DISM /Online /Cleanup-image /StartComponentCleanup
pause