project(dcploymerizepay)
cmake_minimum_required(VERSION  2.6)

#设置编译器 以及 编译参数
SET(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
#add_compile_options(-v)
SET(CMAKE_C_FLAGS "-Wall -v -fPIC -Wl,-Bsymbolic -lm -D__STDINT_LIMITS -D__STDINT_MACROS -D__ANDROID__ -DANDROID")

#头文件路径
#INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/inc/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/inc/ums)

#源码路径
aux_source_directory(./src DIR_SRCS)
message("DIR_SRCS ... " ${DIR_SRCS})

#库文件路径
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Debug) libhello

#输出文件及路径
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
add_executable(dcploymerizepay ${DIR_SRCS})

#添加链接库
set(lib_list "-lpos" "-lpng" "-liconv" "-lfreetype" "-lrt" "-lpthread" "-lz" "-lm" libcrypto.so libdecrdpolymerpey.so libcurl.so libssh2.so libssl.so)
foreach(lib ${lib_list})
	target_link_libraries(${PROJECT_NAME} ${lib})
endforeach(lib)
