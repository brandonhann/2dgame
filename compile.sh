#!/bin/bash
g++ -std=c++11 -o app ./src/*.cpp `sdl2-config --cflags --libs`

