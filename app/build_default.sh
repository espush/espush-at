#!/usr/bin/env bash

SDK_PATH=../../esp_push

cd ${SDK_PATH}/app/espush
./cpfile.sh
if [ $?=0 ]
then
    echo 'cp sdk files complted.'
else
    echo 'cp sdk files failed, aborted.'
    exit 1
fi

cd -
make BOOT=new APP=1 SPI_SIZE_MAP=6
if [ $?=0 ]
then
    echo 'make app1 complted.'
else
    echo 'make app1 failed, aborted.'
    exit 1
fi

make clean


make BOOT=new APP=2 SPI_SIZE_MAP=6
if [ $?=0 ]
then
    echo 'make app2 complted.'
else
    echo 'make app2 failed, aborted.'
    exit 1
fi
