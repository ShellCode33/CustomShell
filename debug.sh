#!/bin/bash
cd "$(dirname "$0")"
g++ -g -std=c++11 shell.cpp commands.cpp -o shell
gdb shell
