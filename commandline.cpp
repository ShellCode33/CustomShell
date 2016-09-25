#include "commandline.h"

using namespace std;

CommandLine::CommandLine(string line)
{
    it_commands_history = commands_history.rend();
    update(line);
}

vector<string> CommandLine::split(string line) const
{
    vector<string> words;

    int i = 0;
    while(line[i] == ' ') //skip spaces
        i++;

    int begin_index = i;


    for(; i < (int)line.size(); i++)
    {
        if(line[i] == ' ' && line[i-1] != '\\')
        {
            words.push_back(line.substr(begin_index, i-begin_index));
            begin_index = i+1;

            while(line[i] == ' ') //skip spaces
                i++;
        }
    }

    if(begin_index < (int)line.size())
        words.push_back(line.substr(begin_index));


    return words;
}

void CommandLine::update(string line)
{
    args.clear();

    if(line == "")
        return;

    vector<string> splitCommand = split(line);
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
    if(command == "ls")
        args.push_back("--color=auto");
}
