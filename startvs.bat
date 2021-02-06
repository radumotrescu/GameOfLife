@echo off
IF exist build ( start build/GameOfLife.sln ) ELSE ( echo "NO BUILD FOLDER" )