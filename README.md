# Shell Custom Written in C++

## Download
```
git clone https://github.com/ShellCode33/ShellCustom.git && cd ShellCustom
```

## Compile
```
g++ -std=c++11 -Wno-unused-variable -Wno-unused-result -Wno-unused-parameter -pthread *.cpp -o shell
```

## Execute
```
chmod +x shell && ./shell
```

## Features
- Handling working directory
- Background processes with &
- Auto completion
- Pipe (two processes only :/)
