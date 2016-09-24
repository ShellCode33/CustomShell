#include "commandline.h"

using namespace std;

CommandLine::CommandLine(string line)
{
    it_commands_history = commands_history.rend();
    update(line);
}

vector<string> CommandLine::split(string line) const
{
    istringstream stream(line);
    vector<string> words;

    while (stream)
    {
        string word;
        stream >> word;

        if(word != "\0") //fix bug, en effet un mot vide était ajouté à la fin du vecteur de strings
            words.push_back(word);
    }

    return words;
}

void CommandLine::update(string line)
{
    args.clear();

    if(line == "")
        return;

    vector<string> splitCommand = split(line);
    command = splitCommand.at(0);

    for(int i = 1; i < splitCommand.size(); i++)
        args.push_back(splitCommand.at(i));
}

std::string CommandLine::getCommand() const
{
    return command;
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
