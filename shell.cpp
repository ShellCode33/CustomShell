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
            words.push_back(word);
    }

    return words;
}

void print(const char *str)
{
    write(STDOUT_FILENO, str, strlen(str)); //Obligé d'utiliser write car elle est async-safe
}

//retourne le début "commun" de 2 strings
string compare(string s1, string s2)
{
    string common = "";

    int i = 0;
    while(s1[i] == s2[i])
        common += s1[i++];

    return common;
}

vector<string> getDirFiles(string path)
{
    vector<string> files;

    DIR *dir = opendir(path.c_str());
    struct dirent *item;

    if(dir != NULL)
    {
        while ((item = readdir(dir)) != NULL)
            if(strcmp(item->d_name, "..") != 0 && strcmp(item->d_name, ".") != 0)
                files.push_back(string(item->d_name));

        closedir (dir);
    }

    else
        cout << "Impossible de lister le contenu du repertoire." << endl;

    return files;
}



//TERMINAL IN ROW MODE
/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios()
{
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= ~ECHO; /* set echo mode */
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

list<string> commands_history;
list<string>::reverse_iterator it_commands_history;
bool arrow_up = false, arrow_down = false;

int key_code_history[2] = {0, 0}; //variable globale car modifiée par le handleCtrlC()
string line = ""; //idem

string processUserInput()
{
    line = "";
    int shiftIndex = 0; //0 = curseur en fin de ligne

    char c = '\0';
    while(c != '\n')
    {
        read(STDIN_FILENO, &c, 1);
        //string test = "code: ";
        //test += to_string((int)c) + "\n";
        //write(STDOUT_FILENO, test.c_str(), test.size());

        if((int)c == 27)
            key_code_history[0] = 27;
        else if((int)c == 91 && key_code_history[0] == 27)
            key_code_history[1] = 91;

        else if(key_code_history[0] == 27 && key_code_history[1] == 91)
        {
            switch((int)c)
            {
                case 65: //flèche haut
                {
                    if(it_commands_history != commands_history.rend())
                    {
                        string clean_line = "";

                        for(int i = 0; i < (int)line.size()+shiftIndex; i++)
                            clean_line += "\b";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += " ";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += "\b";

                        print(clean_line.c_str());
                        line = "";
                        shiftIndex = 0;

                        if(arrow_down)
                            ++it_commands_history;

                        line = *it_commands_history;
                        print(line.c_str());
                        ++it_commands_history;
                        arrow_up = true;
                        arrow_down = false;
                    }

                    break;
                }

                case 66: //flèche bas
                {
                    if(*it_commands_history != commands_history.back())
                    {
                        string clean_line = "";

                        for(int i = 0; i < (int)line.size()+shiftIndex; i++)
                            clean_line += "\b";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += " ";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += "\b";

                        print(clean_line.c_str());
                        line = "";
                        shiftIndex = 0;

                        --it_commands_history;

                        if(it_commands_history != commands_history.rbegin() && arrow_up)
                            --it_commands_history;

                        line = *it_commands_history;
                        print(line.c_str());

                        arrow_up = false;
                        arrow_down = true;
                    }

                    else if(line != "")
                    {
                        string clean_line = "";

                        for(int i = 0; i < (int)line.size()+shiftIndex; i++)
                            clean_line += "\b";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += " ";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += "\b";

                        print(clean_line.c_str());
                        line = "";
                        arrow_up = false;
                        arrow_down = false;
                    }

                    break;
                }

                case 67: //flèche droite
                {
                    if(shiftIndex < 0)
                    {
                        string sub = line.substr(line.size()+shiftIndex, -shiftIndex);
                        shiftIndex++;

                        for(int i = 0; i < -shiftIndex; i++)
                            sub += "\b";

                        write(STDOUT_FILENO, sub.c_str(), sub.size());
                    }
                    break;
                }

                case 68: //flèche gauche

                    if(shiftIndex > -(int)line.size()) //attention line.size() est unsigned !!!
                    {
                        shiftIndex--;
                        write(STDOUT_FILENO, "\b", 1);
                    }

                    break;
            }

            key_code_history[0] = key_code_history[1] = 0;
        }

        else if(c == 9 && shiftIndex == 0) //TAB = AUTOCOMPLETION, pas d'autocompletion si on est pas à la fin de la ligne
        {
            if(line.find('/') != string::npos) //S'il y a un / dans line c'est qu'il s'agit d'un chemin d'accès
            {
              /*
                int command_size = 0; //Variable contenant la taille de la commande (si elle existe)

                while(line[command_size] != ' ' && line[command_size] != '\0' && line[command_size] != '\n')
                  command_size++;

                if(command_size == line.size())
                    command_size = 0;
*/
                string path = "";
                string autocomplete_word = "";

                int slash_index = line.size()-1;
                while(line[slash_index] != '/')
                    slash_index--;

                for(int i = 0; i < slash_index; i++)
                    path += line[i];

                for(int i = slash_index+1; i < line.size(); i++)
                    autocomplete_word += line[i];
/*
                print("path: ");
                print(path.c_str());
                print("autocomplete_word: ");
                print(autocomplete_word.c_str());
*/
                vector<string> files = getDirFiles(path);

                string complete_with = "";
                for(string file : files)
                    if(file.find(autocomplete_word) == 0) //si le fichier commence par autocomplete_word
                        if(complete_with == "")
                            complete_with = file;
                        else
                            complete_with = compare(complete_with, file);

                complete_with = complete_with.substr(autocomplete_word.size(), complete_with.size()-autocomplete_word.size());

                print(complete_with.c_str());
                line += complete_with;
            }

            else //commande ou fichier/dossier "direct" (sans chemin d'accès, dans le répertoire courant)
            {

            }
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
            if(shiftIndex < 0)
            {
                string sub = line.substr(line.size()+shiftIndex+1, -shiftIndex-1);
                line.erase(line.size()+shiftIndex, 1);

                for(int i = 0; i < -shiftIndex; i++)
                    sub += " "; //efface les caractères en trop dans la console

                for(int i = 0; i < -shiftIndex*2-1; i++)
                    sub += "\b";

                print(sub.c_str());

                shiftIndex++;
            }
        }

        else if((c >= 32 && c <= 125) ) //si c'est un caractère "normal"
        {
            if(shiftIndex < 0)
            {
                line.insert(line.size()+shiftIndex, 1, c);
                string sub = line.substr(line.size()+shiftIndex-1, -shiftIndex+1);

                for(int i = 0; i < -shiftIndex; i++)
                    sub += "\b";

                print(sub.c_str());
            }

            else
            {
                line += c;
                write(STDOUT_FILENO, &c, 1);
            }

            key_code_history[0] = key_code_history[1] = 0;
        }

    }

    write(STDOUT_FILENO, "\n", 1);
    return line;
}

char hostname[1024]; //Global car utlisé dans le main ET dans le handleCtrlC()

void handleCtrlC(int signal)
{
    string str = string("^C\n\033[94m") + getenv("USER") +  "@" + hostname + " \033[92m" + workingDirectory + " : \033[0m";
    print(str.c_str());
    key_code_history[0] = key_code_history[1] = 0;
    line = "";
}

int main(int argc, char **argv)
{
    signal(SIGINT, handleCtrlC);
    initTermios();

    string command, line;
    gethostname(hostname, 1023);
    hostname[1023] = '\0';

    it_commands_history = commands_history.rend();

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
            {
                if(line != commands_history.back())
                    commands_history.push_back(line);

                it_commands_history = commands_history.rbegin();
                arrow_up = arrow_down = false;
                it->second(commandLine);
            }

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
                else
                {
                    if(line != *commands_history.rbegin())
                        commands_history.push_back(line);

                    it_commands_history = commands_history.rbegin();
                    arrow_up = arrow_down = false;
                }
            }

            commandLine.clear();
        }

        free(workingDirectory);
    }

    return 0;
}
