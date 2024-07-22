#!/bin/zsh

log()
{
	local INPUT=$1
	
	echo "* install: ${INPUT}"
}

check()
{
	local FILE=$1
	local TYPE=$2

	local DIR=""
	local MSG=""

	if (( TYPE == 1 ))
	then
		DIR=./include/vecs
		MSG="missing file ${FILE}. Run generate_headers script to resolve"
	elif (( TYPE == 2 ))
	then
		DIR=./lib
		MSG="missing file ${FILE}. Compile the library to resolve"
	fi

	if [[ ! -e "${DIR}/${FILE}" ]]
	then
		log "${MSG}"
		exit 1
	fi
}

directory()
{
	local DIR=$1

	if [[ ! -d "${DIR}" ]]
	then
		mkdir -p $DIR
		log "made directory ${DIR}"
	fi
}

LOCAL_INCL=./include/vecs
LOCAL_LIB=./lib
INCL_DIR=/usr/local/include
LIB_DIR=/usr/local/lib

HEADERS=(vecs.hpp templates.hpp)
LIBS=(libvecs.a)

log "installing vecs library"

if [[ ! "$(id -u)" == "0" ]]
then
	log "install script needs to be run as root"
	exit 1
fi

directory $INCL_DIR/vecs
directory $LIB_DIR

for HEADER in "${HEADERS[@]}"
do
	check $HEADER 1
	cp $LOCAL_INCL/$HEADER $INCL_DIR/vecs/$HEADER
	log "installed ${HEADER}"
done

for LIB in "${LIBS[@]}"
do
	check $LIB 2
	cp $LOCAL_LIB/$LIB $LIB_DIR/$LIB
	log "installed ${LIB}"
done

log "done."