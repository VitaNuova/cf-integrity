#!/bin/bash

cd build
# cmake ..
sudo make
cd ..

sudo bash protect.sh
./a.out
