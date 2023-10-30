#! /bin/bash

if [ ! -d build ];then
	mkdir -m 777 build
fi

cd build
cmake ..
make
