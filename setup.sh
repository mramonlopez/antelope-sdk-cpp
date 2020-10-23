#!/bin/bash

echo "Start: Make Secp256k1"
cd secp256k1/
./autogen.sh
./configure --enable-module-recovery --without-bignum
make
cd ..
echo "Done: Make Secp256k1"


echo "Start: Make cryptopp"
cd cryptopp/
make
cd ..
echo "Done: Make cryptopp"

echo "Start: Make abieos"
cd abieos/
git submodule update --init --recursive
mkdir -p build && cd build
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc-8 -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 ..
make abieos
cd ..
cd ..
echo "Done: Make abieos"
