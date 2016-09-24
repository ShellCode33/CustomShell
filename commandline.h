#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <string>
#include <vector>
#include <sstream>
#include <list>

class CommandLine
{
public:
    CommandLine(std::string line);
    std::vector<std::string> split(std::string line) const;
    void update(std::string line);

    std::string getCommand() const;

    std::list<std::string> &getCommandsHistory();
    std::list<std::string>::reverse_iterator &getHistoryIterator();
    void setBegin(); //Remet l'historique au début

    std::vector<std::string> getArgs() const;

private:
    std::string command;
    std::vector<std::string> args;
    std::string path;
    std::string last; //contient élément de la ligne pour l'auto-complétion

    std::list<std::string> commands_history;
    std::list<std::string>::reverse_iterator it_commands_history;
};

#endif // COMMANDLINE_H
