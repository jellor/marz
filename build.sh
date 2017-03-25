#!/bin/bash

BUILD_DIR="build"
BIN_DIR="bin"
LIB_DIR="lib"

build_lib() {
	if [ -d ${LIB_DIR} ] ; then
		rm -rf ${LIB_DIR}
		mkdir ${LIB_DIR}
	else 
		mkdir ${LIB_DIR}
	fi

	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}" ..."
		rm -rf ${BUILD_DIR}
		echo "mkdir "${BUILD_DIR}" ..."
		mkdir ${BUILD_DIR}
	else
		echo "mkdir "${BUILD_DIR}" ..."
		mkdir ${BUILD_DIR}
	fi

	echo "cd "${BUILD_DIR}
	cd ${BUILD_DIR}
	echo "cmake ../src"
	cmake ../src
	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	echo "cd "${LIB_DIR}
	cd ${LIB_DIR}
	if [ -e "libmarz.a" ] ; then
		echo "build lib (libmarz.a) successed"
	else 
		echo "build lib (libmarz.a) failed"
	fi
	cd ..
}

build_bin() {
	echo "============================== build lib =================================="
	build_lib
	echo "============================== build bin =================================="
	if [ -d ${BIN_DIR} ] ; then
		rm -rf ${BIN_DIR}
		mkdir ${BIN_DIR}
	else 
		mkdir ${BIN_DIR}
	fi

	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}" ..."
		rm -rf ${BUILD_DIR}
		echo "mkdir "${BUILD_DIR}" ..."
		mkdir ${BUILD_DIR}
	else
		echo "mkdir "${BUILD_DIR}" ..."
		mkdir ${BUILD_DIR}
	fi

	echo "cd "${BUILD_DIR}
	cd ${BUILD_DIR}
	echo "cmake ../samples"
	cmake ../samples
	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build bin successed"
	else 
		echo "build bin failed"
	fi
}

print_help() {
	echo "Help :"
}

case $1 in 
	lib)
		echo "build lib"
		build_lib
		;;
	bin)
		echo "build bin"
		build_bin
		;;
	*)
		print_help
		;;
esac
