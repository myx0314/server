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
    gcc server/*.c base/*.c -I server/ -I base/ -o server/server
elif [ $1 == client ]
then
    echo "build client"
    gcc client/*.c base/*.c -I client/ -I base/ -o client/client
else
    echo "wrong param!"
    exit
fi

echo "build success!"