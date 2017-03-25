#CMake 最低版本号要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#项目信息
PROJECT(gateway_server)

#查找当前目录下的所有源文件, 并将名称保存到 SRC_LIST_X 变量
AUX_SOURCE_DIRECTORY(./ SRC_LIST_1)
AUX_SOURCE_DIRECTORY(../../protocolbuffer SRC_LIST_2)

#SET(LIBRARY_OUTPUT_PATH ./lib)
SET(EXECUTABLE_OUTPUT_PATH ../bin)

#IF(APPLE)
#ELSEIF(UNIX)
#ENDIF()

#指定头文件的搜索路径, 相当于指定gcc的-I参数
INCLUDE_DIRECTORIES(./ ../../ ../../src ../../src/base ../../src/handler ../../src/network ../../src/reactor ../src/reactor/selector )
#动态链接库或静态链接库的搜索路径, 相当于gcc的-L参数
LINK_DIRECTORIES(../../lib)

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面, 否则会报错

#添加子目录
#ADD_SUBDIRECTORY()

#添加编译参数
ADD_DEFINITIONS(-std=c++11 -Wlogical-op-parentheses -rdynamic -g)

#编译生成程序库
#ADD_LIBRARY(marz STATIC ${SRC_LIST_1})

#SET_TARGET_PROPERTIES(test PROPERTIES COMPILE_FLAGS "-std=c++0x")
#添加链接库, 相同于指定-l参数
#TARGET_LINK_LIBRARIES(marz protobuf)

#指定生成目标
ADD_EXECUTABLE(gateway_server ${SRC_LIST_1} ${SRC_LIST_2})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(gateway_server marz pthread jsoncpp protobuf)
