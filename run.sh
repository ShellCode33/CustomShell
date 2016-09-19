#!/bin/bash
cd "$(dirname "$0")"
g++ -std=c++11 shell.cpp commands.cpp -o shell && ./shell
