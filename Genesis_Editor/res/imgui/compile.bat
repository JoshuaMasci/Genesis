for /r %%i in (*.vert *frag) do C:/VulkanSDK/1.1.121.0/Bin32/glslc.exe %%~nxi -o %%~nxi.spv

pause