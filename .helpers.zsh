#!/bin/zsh

DIR=include/vecs
FILES=($DIR/vecs.hpp $DIR/templates.hpp)
FILE=1
VERSION="0.0.15.3"
TIME=$(date "+%m-%d-%Y %H:%M:%S")
SYS="$(uname -s)"
STAMP="version ${VERSION} generated on ${TIME} with system $(uname -s)"
ALIAS="* generate_header:"

DEPS=(bitset map memory numeric optional set stack string vector)
SRCS=(components device engine entities gui material settings signature synchronization systems)

log()
{
  local INPUT=$1

  echo "${ALIAS} ${INPUT}"
}

set_alias()
{
  local OS=$(uname -s)

  if [[ "${OS}" == "Linux" ]]
  then
    alias make='make config=default_linux-x86_64'
    log "make alias set to 'make config=default_linux-x86_64'"
  elif [[ "${OS}" == "Darwin" ]]
  then
    alias make='make config=default_macos-arm'
    log "make alias set to 'make config=default_macos-arm'"
  fi
}

clear()
{  
  echo -n > $FILES[$FILE]
}

input()
{
  local INPUT=$1
  
  echo $INPUT >> $FILES[$FILE]
}

space()
{
  input ""
}

end_file()
{
  local INPUT=$1
  echo -n $INPUT >> $FILES[$FILE]
}

read_file()
{
  local NAME=$1
  local CLASS=${2:-${(C)NAME}}
  local TEMPLATED=${3:-0}

  local READ=0

  while IFS= read -r LINE
  do
    if [[ $READ == 1 && "${LINE}" == "};" ]]
    then
      input $LINE
      break
    fi
  
    if [[ ( $TEMPLATED == 1 && "${LINE}" == "template <typename T>" ) || "${LINE}" == "class ${CLASS}" ]]
    then
      READ=1
    fi

    if (( READ == 1 ))
    then
      input $LINE
      continue
    fi
  done < src/core/include/$NAME.hpp
}

read_misc()
{
  local NAME=$1
  local START=$2
  local END=$3

  local LINE_NUM=1

  while IFS= read -r LINE
  do
    if (( LINE_NUM >= START ))
    then
      input $LINE
    fi

    LINE_NUM=$((LINE_NUM + 1))

    if (( LINE_NUM > END ))
    then
      break
    fi
  done < src/core/include/$NAME.hpp
}