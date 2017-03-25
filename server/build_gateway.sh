#!/bin/bash

BUILD_DIR="build"
BIN_DIR="bin"
LIB_DIR="lib"

function build_bin() {
	echo "============================== build bin =================================="

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

	echo "cmake ."
	cmake ../gateway

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

function run()  {
	if [ -d ${BIN_DIR} ]; then
		echo "bin directory exist"
	else 
		echo "bin directory not exist"
		build_bin
	fi

	cp -p ./gateway/gateway_svr.conf ./bin

	cd bin;

	./gateway_server

	cd ..	

}

function print_help() {
	echo "Help : "
}

case $1 in 
	bin)
		echo "build bin"
		build_bin
		;;

	run) 
		echo "running ......"
		run
		echo "running successful"
		;;
	*)
		print_help
		;;
esac
