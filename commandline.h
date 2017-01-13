#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>
#include <vector>
#include <sstream>
#include <list>

#include "utils.h"

class CommandLine
{
public:
    CommandLine(std::string line);
    void update(std::string line);

    std::string getCommand() const;

    std::list<std::string> &getCommandsHistory();
    std::list<std::string>::reverse_iterator &getHistoryIterator();
    void setBegin(); //Remet l'historique au début

    std::vector<std::string> getArgs() const;
    void customArgs();

private:
    std::string command;
    std::vector<std::string> args;
    std::string path;

    std::list<std::string> commands_history;
    std::list<std::string>::reverse_iterator it_commands_history;
};

#endif // COMMANDLINE_H
