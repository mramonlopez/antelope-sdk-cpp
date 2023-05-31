if not exist "win32-build" mkdir win32-build
cd win32-build
cmake .. -G"Visual Studio 17 2022"
cd ..