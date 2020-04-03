#!/bin/bash
rm -rf CMakeCache.txt
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CTestTestfile.cmake

echo "----------------------------common_test debug----------------------------"
cmake -DHOST_NAME=linux -DDEBUG=ON
  
make 
 
rm -rf CMakeCache.txt
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CTestTestfile.cmake



