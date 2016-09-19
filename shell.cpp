#include <signal.h>
#include "commands.h"

using namespace std;

vector<string> split(string str){
    istringstream stream(str);
    vector<string> words;

    while (stream)
    {
        string word;
        stream >> word;

        if(word != "\0") //fix bug, en effet un mot vide était ajouté à la fin du vecteur de strings
        {
            if(VERBOSE)
                cout << "word: " << word << endl;

            words.push_back(word);
        }
    }

    return words;
}

void handleCtrlC(int signal)
{
    string str = string("\n\033[94m") + getenv("USER") +  "@" + hostname + " \033[92m" + workingDirectory + " : \033[0m";
    write(STDOUT_FILENO, str.c_str(), str.size()); //Obligé d'utiliser write car les autres fonctions ne sont pas async-safe
}

int main(int argc, char **argv)
{
    signal(SIGINT, handleCtrlC);

    string command, line;
    gethostname(hostname, 1023);
    hostname[1023] = '\0';

    while(true)
    {
        workingDirectory = get_current_dir_name();
        cout << "\033[94m" << getenv("USER") <<  "@" << hostname << " \033[92m" << workingDirectory << " : \033[0m";
        getline(cin, line);

        if(line != "")
        {
            vector<string> commandLine = split(line);
            command = commandLine.at(0);
            commandLine.erase(commandLine.begin());
            std::map<std::string, func_ptr>::iterator it = commands.find(command);

            if(it != commands.end())
                it->second(commandLine);

            else
            {
                bool executed = false;

                if(!access(command.c_str(), X_OK)) //Si le fichier est executable
                {
                    exec(command, commandLine);
                    executed = true;
                }

                else //Si on a pas réussi à l'executer on cherche dans la PATH si le programme y est.
                {
                    for(string p : path)
                    {
                        string prog = p + "/" + command;

                        if(!access(prog.c_str(), X_OK)) //Si le fichier est executable
                        {
                            exec(command, commandLine);
                            executed = true;
                            break;
                        }
                    }
                }

                if(!executed)
                    cout << "Commande \"" << command << "\" introuvable." << endl;
            }

            commandLine.clear();
        }

        free(workingDirectory);
    }

    return 0;
}
