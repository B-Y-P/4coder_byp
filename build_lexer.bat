@echo off

REM For build_one_time.bat:
REM params %1 and %2 are relative to 4coder_byp\ directory (same directory as build_lexer.bat)

call .\custom\bin\build_one_time.bat 4coder_byp_cpp_lexer_gen.cpp  ..\ && ..\one_time.exe

del ..\4coder_byp_cpp_lexer_gen.obj
del ..\one_time.*
REM copy ..\custom\generated\lexer_cpp.* .\generated\lexer_cpp.*
