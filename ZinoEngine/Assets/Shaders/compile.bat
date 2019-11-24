@echo off

D:/VulkanSDK/1.1.126.0/Bin/glslangValidator.exe -V main.vert -o main.vert.spv
D:/VulkanSDK/1.1.126.0/Bin/glslangValidator.exe -V main.frag -o main.frag.spv

IF %ERRORLEVEL% NEQ 0 GOTO error
exit

:error
pause