@echo off
echo %cd%
call custom\bin\build_one_time.bat .\4coder_qol_cpp_lexer_gen.cpp ..\ %1 && ..\one_time.exe || echo failed
call custom\bin\build_one_time.bat .\4coder_qol_lua_lexer_gen.cpp ..\ %1 && ..\one_time.exe || echo failed

del ..\*_lexer_gen.obj
del ..\one_time.*