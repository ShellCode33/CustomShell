#include <signal.h>
#include <termios.h>
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

void print(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str)); //async safe
}

//TERMINAL IN ROW MODE
/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios(int echo)
{
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios()
{
    tcsetattr(0, TCSANOW, &old);
}

//DICO :
//flèche gauche = 68
//flèche droite = 67
//flèche haut = 65
//flèche bas = 66
//tab = 9
//effacer = 127
//suppr = 126

int history[2] = {0, 0}; //varibale globale car modifié par le handleCtrlC()
string line = ""; //idem
string processUserInput()
{
    int shiftIndex = 0; //0 = curseur en fin de ligne

    char c = '\0';
    while(c != '\n')
    {
        read(STDIN_FILENO, &c, 1);
        //string test = "code: ";
        //test += to_string((int)c) + "\n";
        //write(STDOUT_FILENO, test.c_str(), test.size());

        if((int)c == 27)
            history[0] = 27;
        else if((int)c == 91 && history[0] == 27)
            history[1] = 91;

        else if(history[0] == 27 && history[1] == 91)
        {
            switch((int)c)
            {
                case 65: //flèche haut

                    break;

                case 66: //flèche bas

                    break;

                case 67: //flèche droite

                    break;

                case 68: //flèche gauche
                    write(STDOUT_FILENO, "\b", 1);
                    break;
            }

            history[0] = history[1] = 0;
        }

        else if(c == 9) //TAB = AUTOCOMPLETION
        {

        }

        else if(c == 127) //EFFACER
        {
            if(line.size() > 0)
            {
                line.erase(line.end()-1);
                write(STDOUT_FILENO, "\b \b", 3);
            }
        }

        else if(c == 126) //SUPPR
        {

        }

        else if((c >= 32 && c <= 125) ) //si c'est un caractère "normal"
        {
            write(STDOUT_FILENO, &c, 1);
            line += c;
            history[0] = history[1] = 0;
        }

    }

    write(STDOUT_FILENO, "\n", 1);
    return line;
}

void handleCtrlC(int signal)
{
    string str = string("^C\n\033[94m") + getenv("USER") +  "@" + hostname + " \033[92m" + workingDirectory + " : \033[0m";
    write(STDOUT_FILENO, str.c_str(), str.size()); //Obligé d'utiliser write car les autres fonctions ne sont pas async-safe
    history[0] = history[1] = 0;
    line = "";
}

int main(int argc, char **argv)
{
    signal(SIGINT, handleCtrlC);
    initTermios(0);

    string command, line;
    gethostname(hostname, 1023);
    hostname[1023] = '\0';

    while(true)
    {
        workingDirectory = get_current_dir_name();
        string str = string("\033[94m") + getenv("USER") +  "@" + hostname + " \033[92m" + workingDirectory + " : \033[0m";
        print(str.c_str());
        line = processUserInput();

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
