@echo off
REM Set the path to the Python script
SET PYTHON_SCRIPT=copy_file.py

REM Call the Python script
python %PYTHON_SCRIPT%

REM Check if the script executed successfully
IF %ERRORLEVEL% NEQ 0 (
    ECHO Script execution failed
) ELSE (
    ECHO Script executed successfully
)

REM Pause and display a message
pause