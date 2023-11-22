@echo off
echo 清除所有obj pch idb pdb ncb opt plg res sbr ilk suo exp文件，正在清理......

del /f /s /q .\*.obj
del /f /s /q .\*.pch
del /f /s /q .\*.idb
del /f /s /q .\*.pdb
del /f /s /q .\*.ncb 
del /f /s /q .\*.opt 
del /f /s /q .\*.plg
del /f /s /q .\*.res
del /f /s /q .\*.sbr
del /f /s /q .\*.ilk
del /f /s /q .\*.aps
del /f /s /q .\*.exp
del /f /s /q .\*.sdf
del /f /s /q .\*.lib
del /f /s /q .\*Makefile
del /f /s /q .\*.stash
echo 清除文件完成！