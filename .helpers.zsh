#!/bin/zsh

clear()
{
  echo -n > $FILE
}

input()
{
  local LINE=$1
  echo $LINE >> $FILE
}

newline()
{
  input ""
}

read_file()
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