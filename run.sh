#!/bin/bash
cd "$(dirname "$0")"
rm -f *.o
g++ -std=c++11 -Wno-unused-variable -Wno-unused-result -Wno-unused-parameter -pthread *.cpp -o shell && ./shell
