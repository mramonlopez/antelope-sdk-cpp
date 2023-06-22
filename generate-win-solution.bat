if not exist "build" mkdir build
cd build
cmake .. -G"Visual Studio 17 2022" -DCMAKE_CXX_FLAGS="-DABIEOS_NO_INT128=ON -DABIEOS_ONLY_LIBRARY=ON"
cd ..