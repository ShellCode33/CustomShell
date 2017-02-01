#include "userinput.h"
#include "shell.h"

using namespace std;

UserInput::UserInput(Shell &shell, CommandLine &commandLine) : shell(shell), commandLine(commandLine), arrow_up(false), arrow_down(false), key_code_history{0, 0}
{
    //RETOUR A LA LIGNE LORS DEFFACEMENT
}

void UserInput::handleCtrlC()
{
    string str = string("^C\033[1B\033[1000D") + shell.getComputedLineInterface();
    shell.print(str);
    key_code_history[0] = key_code_history[1] = 0;
    line = "";
}

string UserInput::processInput()
{
    line = "";
    int shiftIndex = 0; //0 = curseur en fin de ligne
    std::list<std::string>::reverse_iterator it_commands_history = commandLine.getHistoryIterator();

    char c = '\0';
    while(c != '\n')
    {
        read(STDIN_FILENO, &c, 1);
        /*
        string test = "code: ";
        test += to_string((int)c) + "\n";
        write(STDOUT_FILENO, test.c_str(), test.size());
        */
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
                    if(it_commands_history != commandLine.getCommandsHistory().rend())
                    {
                        string clean_line = "";

                        for(int i = 0; i < (int)line.size()+shiftIndex; i++)
                            clean_line += "\b";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += " ";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += "\b";

                        shell.print(clean_line);
                        line = "";
                        shiftIndex = 0;

                        if(arrow_down)
                            ++it_commands_history;

                        line = *it_commands_history;
                        shell.print(line);
                        ++it_commands_history;
                        arrow_up = true;
                        arrow_down = false;
                    }

                    break;
                }

                case 66: //flèche bas
                {
                    if(it_commands_history != commandLine.getCommandsHistory().rbegin())
                    {
                        string clean_line = "";

                        for(int i = 0; i < (int)line.size()+shiftIndex; i++)
                            clean_line += "\b";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += " ";

                        for(int i = 0; i < (int)line.size(); i++)
                            clean_line += "\b";

                        shell.print(clean_line);
                        line = "";
                        shiftIndex = 0;

                        --it_commands_history;

                        if(it_commands_history != commandLine.getCommandsHistory().rbegin() && arrow_up)
                            --it_commands_history;

                        line = *it_commands_history;
                        shell.print(line);

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

                        shell.print(clean_line);
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
                        shiftIndex++;

                        if((line.size()+shiftIndex+shell.getLineInterfaceSize())%shell.getTerminalSize().ws_col == 0)
                            shell.print("\033[1B\033[1000D");

                        else
                            shell.print("\033[1C");
                    }
                    break;
                }

                case 68: //flèche gauche

                    if(shiftIndex > -(int)line.size()) //attention line.size() est unsigned !!!
                    {
                        if((line.size()+shiftIndex+shell.getLineInterfaceSize())%shell.getTerminalSize().ws_col == 0)
                            shell.print("\033[1A\033[1000C");

                        else
                            shell.print("\033[1D");


                        shiftIndex--;
                    }

                    break;
            }

            key_code_history[0] = key_code_history[1] = 0;
        }

        else if(c == 9 && shiftIndex == 0) //TAB = AUTOCOMPLETION, pas d'autocompletion si on est pas à la fin de la ligne
        {
            int command_size = line.size()-1; //Variable contenant la taille de la commande (si elle existe)

            if(line[command_size] != ' ')
            {
                while(command_size > 1)
                {
                    command_size--;

                    if(line[command_size] == ' ' && line[command_size-1] != '\\')
                        break;
                }
            }

            if(command_size == 1) //aucun programme à 1 caractère
                command_size = 0;

            else if(command_size > 0)
                command_size++; //skip ' '

            if(line.size() == 0) //Si on fait tabulation et qu'il n'y a rien à traiter, on fait un ls
            {
                shell.print('\n');
                return "/bin/ls";
            }

            else if(line.find('/') != string::npos) //S'il y a un / dans line c'est qu'il s'agit d'un chemin d'accès
            {
                string path = "";
                string autocomplete_word = "";

                int slash_index = line.size()-1;
                while(line[slash_index] != '/')
                    slash_index--;

                if(slash_index == command_size)
                    path += "/";

                for(int i = command_size; i < slash_index+1; i++)
                    path += line[i];

                for(int i = slash_index+1; i < (int)line.size(); i++)
                    autocomplete_word += line[i];
/*
                print("\n\npath: ");
                print(path.c_str());
                print("autocomplete_word: ");
                print(autocomplete_word.c_str());
                print("\n\n");
*/
                vector<string> files = Utils::getDirFiles(path);
                string complete_with = "";

                if(autocomplete_word != "")
                {
                    for(string file : files)
                    {
                        if(file.find(autocomplete_word) == 0) //si le fichier commence par autocomplete_word
                        {
                            complete_with = file;
                            break;
                        }
                    }
                }

                //print(to_string(files.size()).c_str());

                if(files.size() == 1)
                {
                    complete_with = files.at(0);
                }

                else if(files.size() > 0)
                {
                    if(autocomplete_word != "")
                    {
                        vector<string> possible_files;

                        for(string file : files)
                        {
                            if(file.find(autocomplete_word) == 0) //si le fichier commence par autocomplete_word
                            {
                                complete_with = Utils::compare(complete_with, file);
                                possible_files.push_back(file);
                            }
                        }

                        if(possible_files.size() > 1)
                        {
                            shell.print("\n");

                            for(string file : possible_files)
                            {
                                shell.print(file);
                                shell.print(" ");
                            }

                            string str = string("\n") + shell.getComputedLineInterface() + line;
                            shell.print(str);
                        }
                    }

                    else
                    {
                        shell.print("\n");

                        for(string file : files)
                        {
                            if(file[0] != '.')
                            {
                                shell.print(file);
                                shell.print(" ");
                            }
                        }

                        string str = string("\n") + shell.getComputedLineInterface() + line;
                        shell.print(str);
                    }
                }

                if(complete_with != "" && complete_with != autocomplete_word)
                    complete_with = complete_with.substr(autocomplete_word.size(), complete_with.size()-autocomplete_word.size());

                else
                    complete_with = "";

                if(complete_with != "")
                {
                    complete_with = Utils::escapeString(complete_with);
                    shell.print(complete_with);
                    line += complete_with;
                }

                if(line[line.size()-1] != '/' && Utils::isDir(Utils::clearEscapedString(line.substr(command_size))))
                {
                    line += "/";
                    shell.print("/");
                }
            }

            else
            {
                vector<string> files = Utils::getDirFiles(shell.getWorkingDirectory());
                vector<string> possible_dirs;

                for(string file : files)
                {
                    if(line.size() != 0 && file.find(line.substr(command_size)) == 0)
                        possible_dirs.push_back(file);

                    else if(line.size() == 0 && file[0] != '.')
                        possible_dirs.push_back(file);


                }

                if(possible_dirs.size() == 1)
                {
                    if(line.size() > 0)
                    {
                        int len = possible_dirs.at(0).size();
                        string sub = possible_dirs.at(0).substr(line.size()-command_size, len);


                        string test = Utils::clearEscapedString((line.substr(command_size) + sub));

                        if(Utils::isDir(Utils::clearEscapedString((line.substr(command_size) + sub))))
                            sub += "/";

                        sub = Utils::escapeString(sub);
                        line += sub;
                        shell.print(sub);
                    }

                    else
                    {
                        string file = possible_dirs.at(0);

                        if(Utils::isDir(Utils::clearEscapedString(file.c_str())))
                            file += "/";

                        line += file;
                        shell.print(file);
                    }
                }

                else if((int)line.size() == command_size || (possible_dirs.size() > 0 && possible_dirs.size() < 50))
                {
                    shell.print("\n");

                    string complete_with = possible_dirs.at(0);

                    for(string dir : possible_dirs)
                    {
                        complete_with = Utils::compare(complete_with, dir);

                        if(dir[0] != '.')
                        {
                            shell.print(dir);
                            shell.print(" ");
                        }
                    }

                    if(line.size() != 0 && complete_with.size() > 0)
                        line += complete_with.substr(line.size()-command_size);

                    string str = string("\n") + shell.getComputedLineInterface();
                    shell.print(str);
                    shell.print(line);
                }

                //Si aucun dossier n'a été trouvé, alors on recherche les programmes du path
                //De plus il ne doit pas y avoir d'espace dans la ligne car sinon cela signifie que ce n'est pas une simple commande et qu'il y a des arguments
                if(line.find(' ') == string::npos && possible_dirs.size() == 0)
                {
                    vector<string> possible_commands;

                    for(string item : shell.getPath())
                    {
                        vector<string> files = Utils::getDirFiles(item);

                        for(string file : files)
                            if(file.find(line.substr(command_size)) == 0)
                                possible_commands.push_back(file);
                    }

                    if(possible_commands.size() == 1)
                    {
                        int len = possible_commands.at(0).size()-line.size()-command_size;
                        string sub = possible_commands.at(0).substr(line.size()-command_size, len);
                        line += sub;
                        shell.print(sub);
                    }

                    else if(possible_commands.size() > 0 && possible_commands.size() < 50) //Pour éviter de flooder le terminal
                    {
                        shell.print("\n");

                        for(string file : possible_commands)
                        {
                            shell.print(file);
                            shell.print(" ");
                        }

                        string str = string("\n") + shell.getComputedLineInterface();
                        shell.print(str);
                        shell.print(line);
                    }
                }
            }
        }

        else if(c == 127) //EFFACER
        {
            if(line.size() > 0 && shiftIndex == 0)
            {
                string str = "";

                if(shell.getLineInterfaceSize()+line.size() >= shell.getTerminalSize().ws_col && (line.size()+shell.getLineInterfaceSize())%(shell.getTerminalSize().ws_col) == 0)
                    str += "\033[1A\033[1000C  \033[1A\033[1000C"; //remove \b

                else
                {
                    str += "\b";

                    if(shell.getLineInterfaceSize()+line.size() == shell.getTerminalSize().ws_col)
                        str += "\033[1C";

                    str += " ";

                    str += "\b";

                    if(shell.getLineInterfaceSize()+line.size() == shell.getTerminalSize().ws_col)
                        str += "\033[1C";

                }



                shell.print(str);
                line.erase(line.end()-1);
            }

            else if((int)line.size() > 0 && -shiftIndex < (int)line.size())
            {
                string sub = line.substr(line.size()+shiftIndex, -shiftIndex);

                if((line.size()+shiftIndex+shell.getLineInterfaceSize())%shell.getTerminalSize().ws_col != 0)
                    shell.print('\b');
                else
                    shell.print("\033[1A\033[1000C");


                for(int i = 0; i < -shiftIndex+1; i++)
                    sub += " "; //efface les caractères en trop dans la console

                for(int i = 0; i < -shiftIndex*2+1; i++)
                    if((line.size()-shiftIndex-i+shell.getLineInterfaceSize())%shell.getTerminalSize().ws_col == 0)
                        sub += "\033[1A\033[1000C";
                    else
                        sub += "\b";


                line.erase(line.end()-1+shiftIndex);
                shell.print(sub);
            }
        }

        else if(c == 126) //SUPPR
        {
            if(shiftIndex < 0)
            {
                string sub = line.substr(line.size()+shiftIndex+1, -shiftIndex-1);
                sub += " "; //efface le caractère en trop dans la console

                for(int i = 0; i < -shiftIndex; i++)
                    if((shell.getLineInterfaceSize()+line.size()-i)%shell.getTerminalSize().ws_col == 0)
                    {
                        if((shell.getLineInterfaceSize()+line.size())%shell.getTerminalSize().ws_col != 0)
                            sub += "\033[1A\033[1000C";

                        else
                            sub += "\b\033[1C";
                    }
                    else
                        sub += "\b";

                shell.print(sub);
                line.erase(line.size()+shiftIndex, 1);
                shiftIndex++;
            }
        }

        else if(c == 3) // CTRL + C
        {
            shiftIndex = 0;
            handleCtrlC();
        }

        else if((c >= 32 && c <= 125) ) //si c'est un caractère "normal"
        {
            if(shiftIndex < 0)
            {
                line.insert(line.size()+shiftIndex, 1, c);
                string sub = line.substr(line.size()+shiftIndex-1);

                for(int i = 0; i < -shiftIndex; i++)
                    if(shell.getLineInterfaceSize()+line.size() > shell.getTerminalSize().ws_col && (shell.getLineInterfaceSize()+line.size()-i)%shell.getTerminalSize().ws_col == 0)
                        sub += "\033[1A\033[1000C";

                    else
                        sub += "\b";

                if(shell.getLineInterfaceSize()+line.size() == shell.getTerminalSize().ws_col)
                    sub += "\033[1C";

                shell.print(sub);
            }

            else
            {
                line += c;
                shell.print(c);
            }

            //fix bug invisible cursor on the right
            if(shell.getLineInterfaceSize()+line.size()+shiftIndex == shell.getTerminalSize().ws_col)
                shell.print(" \033[1D");

            key_code_history[0] = key_code_history[1] = 0;
        }

    }

    shell.print('\n');
    return line;
}

void UserInput::resetArrows()
{
    arrow_down = arrow_up = false;
}
