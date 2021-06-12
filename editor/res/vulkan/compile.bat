for /r %%i in (*.vert *frag) do C:\VulkanSDK\1.2.162.1\Bin\glslc.exe %%~nxi -o %%~nxi.spv

pause