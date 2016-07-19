@cd /d "%~dp0"
set SET_PATH="C:\Program Files\Windows Sidebar App"
set SET_PATH_SERVICE="C:\\Program Files\\Windows Sidebar App\\CaptureScreenService.exe"
mkdir %SET_PATH%
xcopy /c /y /i "Release\*.exe" %SET_PATH%
sc.exe create "CaptureScreenService" binPath=%SET_PATH_SERVICE%
sc.exe start "CaptureScreenService"