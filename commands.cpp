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
    bool backgroundProcess = false;

    if(args.size() > 0 && args[args.size()-1] == "&")
    {
        args.erase(args.end()-1);
        backgroundProcess = true;
    }

    char **params = new char* [args.size()+2];

    params[0] = const_cast<char*>(filename.c_str());

    for(int i = 0; i < (int)args.size(); i++)
        params[i+1] = const_cast<char*>(args[i].c_str());

    params[args.size()+1] = NULL;

    shell.rawMode(false);
    pid_t pid = fork();

    if(pid == 0)
    {
        execv(filename.c_str(), params);
        exit(1); //Ne devrait jamais être appelé en théorie
    }

    if(!backgroundProcess)
    {
        int ret = 0;

        waitpid(pid, &ret, 0);
        ret = WEXITSTATUS(ret);

        if(ret != 0)
            printf("The program returned an error, code : %i\n", ret);
    }

    shell.rawMode(true);
    delete [] params;
}

void Command::pipeProcesses(string line)
{
    vector<string> progs;
    string current = "";

    pid_t pid;
    int prog_pipe[2];

    int i = 0;
    while(i <  line.size())
    {
        current = "";

        while(i < line.size() && line[i] != '|')
            current += line[i++];

        while(current[0] == ' ') //delete spaces
            current.erase(0, 1);

        while(current[current.size()-1] == ' ') //delete spaces
            current.erase(current.size()-1, 1);

        progs.push_back(current);
        i++; //skip |
    }

    if(pipe(prog_pipe))
    {
        cout << "pipe error !!!!" << endl;
        return;
    }

    pid = fork();

    if(pid == 0) //child process
    {
        close(prog_pipe[1]);
        //read pipe[0]
    }

    else if(pid < 0)
    {
        cout << "Fork error !!!!" << endl;
        return;
    }

    else //parent process
    {
        close(prog_pipe[0]);
        //write pipe[1]
    }

}

map<string, func_ptr>& Command::getAvailableCommands()
{
    return commands;
}
