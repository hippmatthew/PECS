#!/bin/zsh

FILE=include/vecs.hpp
VERSION="0.0.8.3"
TIME=$(date "+%m-%d-%Y %H:%M:%S")

FILES=(device engine gui material settings signature synchronization)

clear()
{
  echo -n > $FILE
}

input()
{
  local INPUT=$1
  echo $INPUT >> $FILE
}

newline()
{
  input ""
}

end_file()
{
  local INPUT=$1
  echo -n $INPUT >> $FILE
}

read_file()
{
  local NAME=$1

  local LINE_NUM=1
  local START=0
  local END=0

  if [[ "$NAME" == "extras" ]]
  then
    START=7
    END=43
  else
    while IFS= read -r LINE
    do
      if [[ "${LINE}" == "class ${(C)NAME}" || "${LINE}" == "class GUI" ]]
      then
        START=$LINE_NUM
        continue
      fi

      if [[ "${LINE}" == "};" ]]
      then
        END=$((LINE_NUM + 1))
        break
      fi

      LINE_NUM=$((LINE_NUM + 1))
    done < src/core/include/$NAME.hpp
  fi

  LINE_NUM=1
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