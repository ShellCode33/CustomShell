#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <termios.h>
#include <list>

typedef void(*func_ptr)(std::vector<std::string>); //pointeur de fonction


void help(std::vector<std::string> args);
void shell(std::vector<std::string> args);
void cd(std::vector<std::string> args);
void ls(std::vector<std::string> args);
void echo(std::vector<std::string> args);
void pwd(std::vector<std::string> args);
void exit_prog(std::vector<std::string> args);
void clear(std::vector<std::string> args);
void exec(std::string filename, std::vector<std::string> args);

void initTermios();
void resetTermios();
std::vector<std::string> getDirFiles(std::string path);

static std::string path[] = {
	"/usr/local/sbin",
	"/usr/local/bin",
	"/usr/sbin",
	"/usr/bin",
	"/sbin",
	"/bin"
};

extern char* workingDirectory; //Variable déclarée dans commands.cpp

static std::map<std::string, func_ptr> commands {
    {"help", help},
    {"shell", shell},
    {"echo", echo},
    {"cd", cd},
	{"ls", ls},
    {"pwd", pwd},
    {"exit", exit_prog},
	{"clear", clear}
};

#endif //COMMANDS_H
