@ECHO OFF
SETLOCAL
SET EXIT_STATUS=0
SET ROOT=%cd%
PUSHD third_party

IF NOT EXIST json (
  git clone https://github.com/nlohmann/json.git json
)

SET VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
FOR /f "usebackq delims=" %%i IN (`%VSWHERE% -prerelease -latest -property installationPath`) DO (
  IF EXIST "%%i\Common7\Tools\vsdevcmd.bat" (
    CALL "%%i\Common7\Tools\vsdevcmd.bat"
  )
)
REM IF NOT EXIST Strawberry (
  REM wget https://strawberryperl.com/download/5.32.1.1/strawberry-perl-5.32.1.1-64bit-portable.zip -O strawberry-perl-5.32.1.1-64bit-portable.zip
  REM IF !ERRORLEVEL! LEQ 0 (
    REM MD Strawberry
    REM PUSHD Strawberry
    REM unzip ..\strawberry-perl-5.32.0.1-64bit-portable.zip
    REM POPD
  REM ) ELSE (
    REM SET EXIT_STATUS=1
  REM )
  REM DEL /F /Q strawberry-perl-5.32.0.1-64bit-portable.zip
REM )


SET PATH=%PATH%;%cd%\Strawberry\perl\site\bin;%cd%\Strawberry\perl\bin;%cd%\Strawberry\c\bin
IF NOT EXIST qt-5.15.2 (
  git clone git://code.qt.io/qt/qt5.git qt-5.15.2
  IF %ERRORLEVEL% EQU 0 (
    PUSHD qt-5.15.2
    git checkout 5.15.2
    perl init-repository --module-subset=default
    CALL configure -opensource -static -mp -make libs -make tools ^
      -nomake examples -nomake tests -opengl desktop -no-icu -qt-freetype ^
      -qt-harfbuzz -qt-libpng -qt-pcre -qt-zlib -confirm-license
    SET CL=/MP
    nmake
    DEL qtbase\lib\cmake\Qt5Core\Qt5CoreConfigExtrasMkspecDir.cmake
    COPY NUL qtbase\lib\cmake\Qt5Core\Qt5CoreConfigExtrasMkspecDir.cmake
	POPD
  ) ELSE (
    RD /S /Q qt-5.15.2
    SET EXIT_STATUS=1
  )
)

IF NOT EXIST w3 (
  git clone git@github.com:Z3Prover/z3.git z3 
  IF %ERRORLEVEL% EQU 0 (
    PUSHD z3 
    POPD
  ) ELSE (
    RD /S /Q z3 
    SET EXIT_STATUS=1
  )
)

POPD
ENDLOCAL
