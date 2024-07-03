#!/bin/zsh

FILE=include/vecs.hpp
VERSION="0.0.12.0"
TIME=$(date "+%m-%d-%Y %H:%M:%S")

XTR_START=7
XTR_END=49
DEPS=(map memory optional set stack string vector)
FILES=(components device engine entities gui material settings signature synchronization systems)

clear()
{
  echo -n > $FILE
}

input()
{
  local INPUT=$1
  echo $INPUT >> $FILE
}

space()
{
  input ""
}

end_file()
{
  local INPUT=$1
  echo -n $INPUT >> $FILE
}

read_extras()
{
  local LINE_NUM=1

  while IFS= read -r LINE
  do
    if (( LINE_NUM >= XTR_START ))
    then
      input $LINE
    fi

    LINE_NUM=$((LINE_NUM + 1))

    if (( LINE_NUM > XTR_END ))
    then
      break
    fi
  done < src/core/include/extras.hpp
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