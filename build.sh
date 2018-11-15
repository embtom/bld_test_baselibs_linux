#/bin/sh 
rm build -rf
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release
cmake --build ./

