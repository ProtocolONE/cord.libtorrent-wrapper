@REM /****************************************************************************
@REM ** This file is a part of Syncopate Limited GameNet Application or it parts.
@REM **
@REM ** Copyright (©) 2011 - 2012, Syncopate Limited and/or affiliates. 
@REM ** All rights reserved.
@REM **
@REM ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
@REM ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
@REM ****************************************************************************/

@IF _%1_ == __ @(
    @echo Usage: incgen path  
    @EXIT /B 0;
)

@set TARGETDIR=%1%
@for /f %%i in (%0) do @set CURRENTPUTH=%%~dpi 

@FOR /F "delims==" %%d in ('DIR /ogne /ad /b /s %TARGETDIR%') DO @(
    @cd %%d
    @FORFILES /m *.h /c "CMD /c ECHO #include @file > @fname"
)

@cd %TARGETDIR%
@FORFILES /m *.h /c "CMD /c ECHO #include @file > @fname"
@cd %CURRENTPUTH%
@call echo Reset error level hack

@EXIT /B 0
