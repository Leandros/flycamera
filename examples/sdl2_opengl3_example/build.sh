#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "$0")")"

cc -o app main.c -std=c99 -g -I../glad/include -lSDL2 -lGL -ldl -lm
