#include "commands.h"

using namespace std;

char* workingDirectory;

void help(vector<string> args)
{
    cout << "\t- echo : affiche le texte qui suit" << endl;
    cout << "\t- cd : change le dossier courant" << endl;
    cout << "\t- ls : liste le contenu d'un répertoire" << endl;
    cout << "\t- shell : lance un shell" << endl;
    cout << "\t- pwd : affiche le répertoire de travail" << endl;
    cout << "\t- help : affiche cette aide" << endl;
    cout << "\t- exit : quitte le shell courant" << endl;
}

void echo(vector<string> args)
{
    for(string str : args)
        cout << str << " ";

    cout << endl;
}

void cd(vector<string> args)
{
    string dirName = "";

    for(string str : args)
        dirName += str + " ";

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

void pwd(vector<string> args)
{
    if(workingDirectory != NULL)
        cout << workingDirectory << endl;
}

void exit_prog(vector<string> args)
{
    exit(0);
}

void shell(vector<string> args)
{
    system(getenv("SHELL"));
}

void ls(vector<string> args)
{
    string dirName = workingDirectory;

    if(args.size() > 0)
        dirName = args.at(0);

    DIR *dir = opendir(dirName.c_str());
    struct dirent *item;


    if(dir != NULL)
    {
        while ((item = readdir(dir)) != NULL)
            if(strcmp(item->d_name, "..") != 0 && strcmp(item->d_name, ".") != 0)
                cout << item->d_name << " ";

        closedir (dir);
        cout << endl;
    }

    else
        cout << "Impossible de lister le contenu du repertoire." << endl;
}

void clear(vector<string> args)
{
    cout << "\033[2J";
    cout << "\033[1;1H";
}

void exec(string filename, vector<string> args)
{
    string cmd = filename;

    for(string s: args)
        cmd += " " + s;

    system(cmd.c_str());
}
