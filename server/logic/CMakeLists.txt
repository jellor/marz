#CMake 最低版本号要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#项目信息
PROJECT(logic)

AUX_SOURCE_DIRECTORY(./ SRC_LIST_1)
AUX_SOURCE_DIRECTORY(../../protocolbuffer SRC_LIST_2)
AUX_SOURCE_DIRECTORY(./data/db SRC_LIST_3)
AUX_SOURCE_DIRECTORY(./data/cache SRC_LIST_4)
AUX_SOURCE_DIRECTORY(./group SRC_LIST_5)
AUX_SOURCE_DIRECTORY(./login SRC_LIST_6)
AUX_SOURCE_DIRECTORY(./message SRC_LIST_7)
AUX_SOURCE_DIRECTORY(./relation SRC_LIST_8)
AUX_SOURCE_DIRECTORY(./session SRC_LIST_9)
AUX_SOURCE_DIRECTORY(./user SRC_LIST_10)

#SET(LIBRARY_OUTPUT_PATH ./)
SET(EXECUTABLE_OUTPUT_PATH ../bin)

IF(APPLE)
	SET(MYSQL_INCLUDE_DIR /usr/local/mysql/include)
	SET(MYSQL_LIB_DIR /usr/local/mysql/lib)
ELSEIF(UNIX)

ENDIF()

#指定头文件的搜索路径, 相当于指定gcc的-I参数
INCLUDE_DIRECTORIES(./ ../../ ../../src ../../src/base ../../src/handler ../../src/network ../../src/reactor ../src/reactor/selector ${MYSQL_INCLUDE_DIR} ./data/db ./data/cache ./group ./login ./ message ./relation ./session ./user)
#动态链接库或静态链接库的搜索路径, 相当于gcc的-L参数
LINK_DIRECTORIES(../../lib ${MYSQL_LIB_DIR})

#ADD_XXXX必须在TARGET_LINK_LIBRARIES前面, 否则会报错

#添加子目录
#ADD_SUBDIRECTORY()

#添加编译参数
ADD_DEFINITIONS(-std=c++11 -rdynamic -g)

#编译生成程序库
#ADD_LIBRARY()

#指定生成目标
ADD_EXECUTABLE(group_server ${SRC_LIST_5})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(group_server marz pthread protobuf mysqlclient hiredis curl)

#指定生成目标
ADD_EXECUTABLE(login_server ${SRC_LIST_6})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(login_server marz pthread protobuf mysqlclient hiredis curl)


#指定生成目标
ADD_EXECUTABLE(message_server ${SRC_LIST_7})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(message_server marz pthread protobuf mysqlclient hiredis curl)

#指定生成目标
ADD_EXECUTABLE(relation_server ${SRC_LIST_8})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(relation_server marz pthread protobuf mysqlclient hiredis curl)

#指定生成目标
ADD_EXECUTABLE(session_server ${SRC_LIST_9})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(session_server marz pthread protobuf mysqlclient hiredis curl)

#指定生成目标
ADD_EXECUTABLE(user_server ${SRC_LIST_10})
#添加链接库, 相同于指定-l参数
TARGET_LINK_LIBRARIES(user_server marz pthread protobuf mysqlclient hiredis curl)
