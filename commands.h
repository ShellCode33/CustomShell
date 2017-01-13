#ifndef COMMANDS_H
#define COMMANDS_H

#include <unistd.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <signal.h>
#include <termios.h>
#include <sys/wait.h>

#include <fstream>
#include <thread>
#include <mutex>

class Shell;

class Command
{
typedef void(Command::*func_ptr)(std::vector<std::string>); //pointeur de methode

public:
    Command(Shell &shell);
    void help(std::vector<std::string> args);
    void runshell(std::vector<std::string> args);
    void cd(std::vector<std::string> args);
    //void ls(std::vector<std::string> args); // /bin/ls
    void echo(std::vector<std::string> args);
    void pwd(std::vector<std::string> args);
    void delay(std::vector<std::string> args);
    void exit_prog(std::vector<std::string> args);
    void clear(std::vector<std::string> args);
    void exec(std::string filename, std::vector<std::string> args);

    std::vector<std::string> getDirFiles(std::string path);
    std::string clearEscapedString(std::string str);
    std::map<std::string, func_ptr> &getAvailableCommands();
    bool pipeProcesses(std::string line); //retourne true si tout s'est bien passé
    bool redirectOutputFile(std::string line);
    bool isInteger(const std::string &s);

private:
    Shell &shell; //Les fonctions de cette classe ne peuvent pas être statiques à cause de cet attribut.
    std::map<std::string, func_ptr> commands = {
                                                 {"help", &Command::help},
                                                 {"shell", &Command::runshell},
                                                 {"echo", &Command::echo},
                                                 {"cd", &Command::cd},
                                                 //{"ls", ls}, //Autant utiliser /bin/ls
                                                 {"pwd", &Command::pwd},
                                                 {"delay", &Command::delay},
                                                 {"exit", &Command::exit_prog},
                                                 {"clear", &Command::clear}
                                             };
    std::mutex cout_mutex;



    void delay_t(std::vector<std::string> args);
};

#endif //COMMANDS_H
