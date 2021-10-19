@echo off
IF NOT exist ..\test_build (
        mkdir ..\test_build
        xcopy /E ..\themes\   ..\test_build\themes\
        xcopy /E ..\fonts\    ..\test_build\fonts\
        copy ..\4ed.exe       ..\test_build\4ed.exe
        copy ..\4ed_app.dll   ..\test_build\4ed_app.dll
        copy ..\config.4coder ..\test_build\config.4coder
)
pushd ..\test_build
call ..\custom\bin\buildsuper_x64-win.bat ..\4coder_byp\4coder_byp.cpp %1
call ..\4coder_byp\apply.bat
popd
