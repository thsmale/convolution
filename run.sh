#!/bin/bash

gcc main.c data.c fio.c convolution.c debug.c /usr/local/lib/libjpeg.a 

#Do not continue if program did not compile w/out errors or warnings
status=$?
if [ ! $status -eq 0 ]
then
	exit 1
fi

#echo "Compiled successfuly"

if [ ! $# -eq 1 ]
  then
    echo "Usage: ./run.sh filename|directory" >&2
    exit 1
fi

./a.out  $1
