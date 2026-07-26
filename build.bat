@echo off
set byp_dir=%cd%
IF NOT exist ..\test_build (
	mkdir ..\test_build
	xcopy /E ..\themes\   ..\test_build\themes\
	xcopy /E ..\fonts\    ..\test_build\fonts\
	copy ..\4ed.exe       ..\test_build\4ed.exe
	copy ..\4ed_app.dll   ..\test_build\4ed_app.dll
	copy ..\config.4coder ..\test_build\config.4coder
)
pushd ..\test_build
call %byp_dir%\4coder_qol\custom\bin\buildsuper_x64-win.bat %byp_dir%\4coder_qol\4coder_qol.cpp %1
copy .\custom_4coder.* ..\custom_4coder.*
copy .\custom_4coder.* %byp_dir%\custom_4coder.*
popd
