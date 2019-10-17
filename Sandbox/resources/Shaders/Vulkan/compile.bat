for /r %%i in (*.vert *frag) do C:/VulkanSDK/1.1.121.0/Bin32/glslangValidator.exe -V %%~nxi -o %%~nxi.spv

pause