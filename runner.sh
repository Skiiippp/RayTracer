#!/usr/bin/bash

g++ -pthread main.cpp
./a.out > image.ppm
eog image.ppm
