#!/bin/bash
set -e

if [ $# != 1 ]
then
    echo "wrong param num!"
    exit
fi

if [ $1 == server ]
then
    echo "build server"
    gcc server/*.c -I server/ -I base/ -o server/server
elif [ $1 == client ]
then
    echo "build client"
else
    echo "wrong param!"
    exit
fi

echo "build success!"