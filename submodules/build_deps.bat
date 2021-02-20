cd yaml-cpp
mkdir build
cd build
cmake -A x64 ..
cmake --build . --clean-first --config Debug
cmake --build . --clean-first --config Release