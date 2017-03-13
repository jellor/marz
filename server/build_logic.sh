#!/bin/bash

BUILD_DIR="build"
BIN_DIR="bin"
LIB_DIR="lib"

LOGIN_SVR="login_server"
GROUP_SVR="group_server"
MESSAGE_SVR="message_server"
RELATION_SVR="relation_server"
SESSION_SVR="session_server"
USER_SVR="user_server"

function build_group() {
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

	echo "cmake ../logic/group"
	cmake ../logic/group

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build group successed"
	else 
		echo "build group failed"
	fi
}

function run_group()  {
	if [ -f ${BIN_DIR}/${GROUP_SVR} ]; then
		echo ${GROUP_SVR}" exist"
	else 
		echo ${GROUP_SVR}" exist"
		build_group
	fi

	cp -p ./logic/group/group_svr.conf ./bin

	cd bin;

	./${GROUP_SVR}

	cd ..	

}

function build_login() {
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

	echo "cmake ../logic/login"
	cmake ../logic/login

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build login successed"
	else 
		echo "build login failed"
	fi
}

function run_login()  {
	if [ -f ${BIN_DIR}/${LOGIN_SVR} ]; then
		echo ${LOGIN_SVR}" exist"
	else 
		echo ${LOGIN_SVR}" not exist"
		build_login
	fi

	cp -p ./logic/login/login_svr.conf ./bin

	cd bin;

	./${LOGIN_SVR}

	cd ..	
}

function build_message() {
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

	echo "cmake ../logic/message"
	cmake ../logic/message

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build message successed"
	else 
		echo "build message failed"
	fi
}

function run_message()  {
	if [ -f ${BIN_DIR}/${MESSAGE_SVR} ]; then
		echo ${MESSAGE_SVR}" exist"
	else 
		echo ${MESSAGE_SVR}" not exist"
		build_message
	fi

	cp -p ./logic/message/message_svr.conf ./bin

	cd bin;

	./${MESSAGE_SVR}

	cd ..	
}

function build_relation() {
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

	echo "cmake ../logic/relation"
	cmake ../logic/relation

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build login successed"
	else 
		echo "build login failed"
	fi
}

function run_relation()  {
	if [ -f ${BIN_DIR}/${RELATION_SVR} ]; then
		echo ${RELATION_SVR}" exist"
	else 
		echo ${RELATION_SVR}" not exist"
		build_relation
	fi

	cp -p ./logic/relation/relation_svr.conf ./bin

	cd bin;

	./${RELATION_SVR}

	cd ..	
}

function build_session() {
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

	echo "cmake ../logic/session"
	cmake ../logic/session

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build session successed"
	else 
		echo "build session failed"
	fi
}

function run_session()  {
	if [ -f ${BIN_DIR}/${SESSION_SVR} ]; then
		echo ${SESSION_SVR}" exist"
	else 
		echo ${SESSION_SVR}" not exist"
		build_session
	fi

	cp -p ./logic/session/session_svr.conf ./bin

	cd bin;

	./${SESSION_SVR}

	cd ..	
}

function build_user() {
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

	echo "cmake ../logic/user"
	cmake ../logic/user

	echo "make"
	make 
	
	echo "cd .."
	cd ..
	if [ -d ${BUILD_DIR} ] ; then
		echo "rm "${BUILD_DIR}
		rm -rf ${BUILD_DIR}
	fi

	if [ -s ${BIN_DIR} ] ; then
		echo "build user successed"
	else 
		echo "build user failed"
	fi
}

function run_user()  {
	if [ -f ${BIN_DIR}/${USER_SVR} ]; then
		echo ${USER_SVR}" exist"
	else 
		echo ${USER_SVR}" not exist"
		build_session
	fi

	cp -p ./logic/user/user_svr.conf ./bin

	cd bin;

	./${USER_SVR}

	cd ..	
}

function print_help() {
	echo "Help : "
}

case $1 in 
	bin)
		echo "build bin"
		#build_bin
		;;

	run) 
		echo "running ......"
		#run
		echo "running successful"
		;;

	group_bin)
		echo "build group"
		build_group
		;;

	group_run)
		echo "run group"
		run_group
		;;

	login_bin)
		echo "build login"
		build_login
		;;

	login_run)
		echo "run login"
		run_login
		;;

	message_bin)
		echo "build message"
		build_message
		;;

	message_run)
		echo "run message"
		run_message
		;;

	relation_bin)
		echo "build relation"
		build_relation
		;;

	relation_run)
		echo "run relation"
		run_relation
		;;

	session_bin)
		echo "build session"
		build_session
		;;

	session_run)
		echo "run session"
		run_session
		;;

	user_bin)
		echo "build user"
		build_user
		;;

	user_run)
		echo "run user"
		run_user
		;;

	*)
		print_help
		;;
esac
