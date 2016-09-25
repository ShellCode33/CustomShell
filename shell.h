#ifndef SHELL_H
#define SHELL_H

#include <string>

#include "commandline.h"
#include "userinput.h"
#include "commands.h"
#include "utils.h"

class Shell
{
typedef void(Command::*func_ptr)(std::vector<std::string>); //pointeur de fonction

public:
    Shell();
    ~Shell() = default;
    void run();
    void rawMode(bool enable);
    void print(std::string str) const;
    void print(char *str) const;
    void print(char c) const;
    void print(int i) const;
    std::string getWorkingDirectory() const;
    std::string getHostname() const;
    std::vector<std::string> getPath() const;
    std::string getComputedLineInterface() const;
    int getLineInterfaceSize() const;

private:
    struct termios old, new1;
    Command command; //Contient toutes les commandes de base ainsi que leurs actions
    CommandLine commandLine;
    UserInput user;

    std::vector<std::string> path = {
        "/usr/local/sbin",
        "/usr/local/bin",
        "/usr/sbin",
        "/usr/bin",
        "/sbin",
        "/bin"
    };

    std::string workingDirectory;
    std::string hostname;

};

#endif // SHELL_H
