#!/bin/bash

if [ -z "$1" ]
  then
    echo "1st argument missing: [serial-port number]"
    exit 1
fi
if [ -z "$2" ]
  then
    echo "2nd argument missing: [command]"
    exit 1
fi


idf.py -p /dev/ttyS"$1" -b 921600 "$2"
