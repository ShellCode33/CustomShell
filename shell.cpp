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

void Shell::print(char const *str) const
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

//Replace some pieces of path like HOME, VIDEOS, PICTURES etc... in order to shorten the path
string Shell::getComputedLineInterface() const
{
    string computed = string("\033[1m\033[93m") + getenv("USER") +  "@" + getHostname();
    string wd = getWorkingDirectory();
    string base = string("/home/") + getenv("USER");
    size_t index = string::npos;

    if((index = wd.find(base)) != string::npos)
    {
        computed += " \033[91m";
        wd.replace(index, base.size(), "HOME");
        wd.insert(index+4, "\033[92m");
    }

    else
        computed += " \033[92m\033[92m";


    computed += wd + " : \033[0m";
    return computed;
}

int Shell::getLineInterfaceSize() const
{
    return getComputedLineInterface().size() - sizeof("\033[1m\033[93m") - sizeof(" \033[92m\033[92m") - sizeof(" : \033[0m") + 7; //pourquoi + 7 ? bonne question ! Aucune idée pour le moment... Décalage étrange
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

        //Change flags to get rawmode
        new1.c_iflag |= IGNBRK;
        new1.c_lflag &= ~(ICANON | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);
        new1.c_cc[VMIN] = 1;
        new1.c_cc[VTIME] = 0;

        tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    }

    else
        tcsetattr(0, TCSANOW, &old);
}

void Shell::run()
{
    string line;
    chdir((string("/home/") + getenv("USER")).c_str());

    while(true)
    {
        workingDirectory = string(get_current_dir_name());
        print(getComputedLineInterface());

        line = user.processInput();

        if(line != "")
        {
            commandLine.update(line);
            std::map<std::string, func_ptr>::iterator it = command.getAvailableCommands().find(commandLine.getCommand());

            if(line.find('|') != string::npos)
            {
                bool pipe_ret = command.pipeProcesses(line);

                if(pipe_ret && line != commandLine.getCommandsHistory().back())
                    commandLine.getCommandsHistory().push_back(line);
            }

            else if(commandLine.getCommand().size() > 0 && line.find('>') != string::npos)
            {
                bool redirect_ret = command.redirectOutputFile(line);

                if(redirect_ret && line != commandLine.getCommandsHistory().back())
                    commandLine.getCommandsHistory().push_back(line);
            }

            else if(it != command.getAvailableCommands().end())
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
                if(!execCommand(commandLine.getCommand(), commandLine.getArgs()))
                    cout << "\"" << commandLine.getCommand() << "\" : command not found." << endl;
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

struct winsize Shell::getTerminalSize()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size); //On actualise la taille du terminal (au cas où celle-ci changerait), ca peut sembler overkill de mettre à jour à chaque fois qu'on récupère la taille, mais c'est préférable pour éviter une race condition
    return window_size;

}

void Shell::addJob(pid_t pid)
{
    bg_jobs.push_back(pid);
}

void Shell::removeJob(pid_t pid)
{
    bg_jobs.remove(pid);
}

Command & Shell::getCommand()
{
    return command;
}

bool Shell::execCommand(const string cmd, const std::vector<string> &args)
{
    bool executed = false;

    if(!Utils::isDir(cmd)) //On s'assure que cmd n'est pas un dossier (histoire de pas executer un dossier x) )
    {
        if(!access(cmd.c_str(), X_OK)) //Si le fichier est executable
        {
            command.exec(cmd, args);
            executed = true;
        }

        else //Si on a pas réussi à l'executer on cherche dans la PATH si le programme y est.
        {
            for(string p : path)
            {
                string prog = p + "/" + cmd;
                prog = Utils::clearEscapedString(prog);

                if(!access(prog.c_str(), X_OK)) //Si le fichier est executable
                {
                    command.exec(prog, args);
                    executed = true;
                    break;
                }
            }
        }
    }

    else
        cout << "You're trying to execute a directory : \"" << cmd << "\"" << endl;

    return executed;
}
