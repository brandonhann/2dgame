#!/bin/bash
g++ -std=c++11 -o program ./src/*.cpp `sdl2-config --cflags --libs` -lSDL2_image
