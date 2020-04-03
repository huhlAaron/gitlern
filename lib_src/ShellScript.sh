#!/bin/bash
rm -rf CMakeCache.txt
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CTestTestfile.cmake

if [[ $@ =~ "-linux" ]]
then
	echo "----------------------------cmake linux debug----------------------------"
	cmake -DHOST_NAME=linux -DDEBUG=ON
elif [[ $@ =~ "-help" ]] 
then
	echo " "
	echo "  不带参数    交叉编译android环境下release二进制文件"
	echo "  -d          交叉编译android环境下debug类型二进制文件"
	echo "  -linux      交叉编译linux环境下debug二进制文件"
	echo " "
	exit 
elif [[ $@ =~ "-d" ]] 
then
	echo "----------------------------cmake android debug----------------------------"
	cmake -DHOST_NAME=android -DDEBUG=ON -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake\
      -DANDROID_NDK=$ANDROID_NDK_HOME\
      -DANDROID_ABI="armeabi-v7a"\
	  #-DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9\
	  #-DANDROID_NATIVE_API_LEVEL=21
	  #-DANDROID_STL=gnustl_static
else      
	echo "----------------------------cmake android release----------------------------"
	cmake  -DHOST_NAME=android -DDEBUG=NO -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake\
      -DANDROID_NDK=$ANDROID_NDK_HOME\
      -DANDROID_ABI="armeabi-v7a"    
	  #-DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9\
	  #-DANDROID_NATIVE_API_LEVEL=21
	  #-DANDROID_STL=gnustl_static
fi
	  
make 
 
rm -rf CMakeCache.txt
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf Makefile
rm -rf CTestTestfile.cmake
