@echo off
setlocal

REM Launch mario_scene from Windows via WSL.
REM This works even though mario_scene is a Linux ELF binary.

wsl bash -lc "cd /home/yomna/mario/build && ./mario_scene"

if errorlevel 1 (
  echo.
  echo Failed to launch mario_scene.
  echo Make sure it is built first:
  echo   cd /home/yomna/mario/build ^&^& cmake .. ^&^& cmake --build .
  echo.
  pause
)

endlocal
