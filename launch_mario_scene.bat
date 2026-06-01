@echo off
setlocal

REM Launch mario_scene from Windows via WSL.
REM This rebuilds the project cleanly before running the latest version.

wsl bash -lc "cd /home/yomna/mariofinal/mario/build && cmake .. && cmake --build . --clean-first && ./mario_scene"

if errorlevel 1 (
  echo.
  echo Failed to build or launch mario_scene.
  echo Make sure WSL and the project are set up correctly.
  echo   cd /home/yomna/mariofinal/mario/build ^&^& cmake .. ^&^& cmake --build . --clean-first
  echo.
  pause
)

endlocal
