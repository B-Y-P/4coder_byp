@echo off
call custom\bin\build_one_time.bat .\4coder_qol_cpp_lexer_gen.cpp ..\ %1
del ..\4coder_qol_cpp_lexer_gen.obj
..\one_time.exe
del ..\one_time.*