#include "commands.h"
#include "shell.h"

using namespace std;

typedef void(Command::*func_ptr)(std::vector<std::string>); //pointeur de fonction

Command::Command(Shell &shell) : shell(shell)
{

}

void Command::help(vector<string> args)
{
    cout << "\t- echo : affiche le texte qui suit" << endl;
    cout << "\t- cd : change le dossier courant" << endl;
    cout << "\t- ls : liste le contenu d'un répertoire" << endl;
    cout << "\t- shell : lance un shell" << endl;
    cout << "\t- pwd : affiche le répertoire de travail" << endl;
    cout << "\t- help : affiche cette aide" << endl;
    cout << "\t- exit : quitte le shell courant" << endl;
}

void Command::echo(vector<string> args)
{
    for(string str : args)
        cout << str << " ";

    cout << endl;
}

void Command::cd(vector<string> args)
{
    string dirName = "";

    for(string str : args)
        dirName += str + " ";

    dirName = Utils::clearEscapedString(dirName);

    dirName.erase(dirName.end()-1);

    if(!args.size())
        chdir(getenv("HOME"));

    else if(chdir(dirName.c_str()) == -1)
    {
        switch(errno)
        {
            case ENOENT:
            case ENOTDIR:
                cout << "Le dossier n'existe pas." << endl;
                break;

            case EACCES:
                cout << "Accès interdit." << endl;
                break;

            default:
                cout << "Une erreur est survenue." << endl;
                break;
        }
    }
}

void Command::pwd(vector<string> args)
{
    if(shell.getWorkingDirectory() != "")
        cout << shell.getWorkingDirectory() << endl;
    else
        cout << "pwd error." << endl;
}

void Command::exit_prog(vector<string> args)
{
    shell.rawMode(false);
    exit(0);
}

void Command::runshell(vector<string> args)
{
    shell.rawMode(false);
    system(getenv("SHELL"));
    shell.rawMode(true);
}

/*
//Autant utiliser /bin/ls
void Command::ls(vector<string> args)
{
    string dirName = "";

    if(args.size() == 0)
        dirName = workingDirectory;

    for(string str : args)
        dirName += str + " ";

    if(args.size() > 0)
    {
        dirName = clearEscapedString(dirName);
        dirName.erase(dirName.size()-1);
    }

    vector<string> files = getDirFiles(dirName);

    if(files.size() > 0)
    {
        for(string file : files)
            cout << file << " ";
    }

    else
        cout << "Aucun fichier ou dossier.";

    cout << endl;
}
*/

void Command::clear(vector<string> args)
{
    write(STDOUT_FILENO, "\033[2J\033[1;1H", sizeof("\033[2J\033[1;1H")); //async safe
}

void Command::exec(string filename, vector<string> args)
{
    string cmd = filename;

    for(string s: args)
        cmd += " " + s;

    cmd = Utils::clearEscapedString(cmd);

    shell.rawMode(false);
    system(cmd.c_str());
    shell.rawMode(true);
}

map<string, func_ptr>& Command::getAvailableCommands()
{
    return commands;
}
