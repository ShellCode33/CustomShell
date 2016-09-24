#include "commands.h"
#include "shell.h"

using namespace std;

Shell::Shell() : command(*this), commandLine(""), user(*this, commandLine)
{
    rawMode(true);

    char *tmp = (char*) calloc(1024, sizeof(char));
    gethostname(tmp, 1023);
    hostname = string(tmp);
    free(tmp);
}

void Shell::print(string str) const
{
    write(STDOUT_FILENO, str.c_str(), str.size()); //async-safe
}

void Shell::print(char *str) const
{
    write(STDOUT_FILENO, str, strlen(str)); //async-safe
}

void Shell::print(char c) const
{
    write(STDOUT_FILENO, &c, 1);
}

void Shell::print(int i) const
{
    print(to_string(i));
}

string Shell::getWorkingDirectory() const
{
    return workingDirectory;
}

string Shell::getHostname() const
{
    return hostname;
}

vector<string> Shell::getPath() const
{
    return path;
}

void Shell::rawMode(bool enable)
{
    if(enable)
    {
        tcgetattr(0, &old); /* grab old terminal i/o settings */
        new1 = old; /* make new settings same as old settings */
        new1.c_lflag &= ~ICANON; /* disable buffered i/o */
        new1.c_lflag &= ~ECHO; /* disable echo mode */
        //new1.c_cc[VINTR] = 3; // Catch CTRL + C
        tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    }

    else
        tcsetattr(0, TCSANOW, &old);
}

void Shell::run()
{
    string line;

    while(true)
    {
        workingDirectory = string(get_current_dir_name());
        string str = string("\033[94m") + getenv("USER") +  "@" + hostname + " \033[92m" + workingDirectory + " : \033[0m";
        print(str);

        line = user.processInput();

        if(line != "")
        {
            commandLine.update(line);
            std::map<std::string, func_ptr>::iterator it = command.getAvailableCommands().find(commandLine.getCommand());

            if(it != command.getAvailableCommands().end())
            {
                if(line != commandLine.getCommandsHistory().back())
                    commandLine.getCommandsHistory().push_back(line);

                commandLine.setBegin();
                user.resetArrows();
                func_ptr func = it->second;
                (command.*(func))(commandLine.getArgs()); //On appelle la methode de la classe Command
            }

            else
            {
                bool executed = false;

                if(!access(commandLine.getCommand().c_str(), X_OK)) //Si le fichier est executable
                {
                    command.exec(commandLine.getCommand(), commandLine.getArgs());
                    executed = true;
                }

                else //Si on a pas réussi à l'executer on cherche dans la PATH si le programme y est.
                {
                    for(string p : path)
                    {
                        string prog = p + "/" + commandLine.getCommand();
                        prog = Utils::clearEscapedString(prog);

                        if(!access(prog.c_str(), X_OK)) //Si le fichier est executable
                        {
                            command.exec(prog, commandLine.getArgs());
                            executed = true;
                            break;
                        }
                    }
                }

                if(!executed)
                    cout << "Commande \"" << commandLine.getCommand() << "\" introuvable." << endl;
                else
                {
                    if(line != commandLine.getCommandsHistory().back())
                        commandLine.getCommandsHistory().push_back(line);

                    commandLine.setBegin();
                    user.resetArrows();
                }
            }
        }
    }
}
