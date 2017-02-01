#include "commandline.h"

using namespace std;

CommandLine::CommandLine(string line)
{
    it_commands_history = commands_history.rend();
    update(line);
}

void CommandLine::update(string line)
{
    args.clear();

    if(line == "")
        return;

    vector<string> splitCommand = Utils::parse(line);

    if(splitCommand.size() == 0)
        return;

    command = splitCommand.at(0);
    customArgs();

    for(int i = 1; i < (int)splitCommand.size(); i++)
        args.push_back(splitCommand.at(i));
}

std::string CommandLine::getCommand() const
{
    return Utils::clearEscapedString(command);
}

std::list<std::string>& CommandLine::getCommandsHistory()
{
    return commands_history;
}

list<string>::reverse_iterator& CommandLine::getHistoryIterator()
{
    return it_commands_history;
}

void CommandLine::setBegin()
{
    it_commands_history = commands_history.rbegin();
}

std::vector<std::string> CommandLine::getArgs() const
{
    return args;
}

void CommandLine::customArgs()
{
    if(command == "ls" || command == "/bin/ls")
        args.push_back("--color=auto");
}
