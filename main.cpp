#include "shell.h"

void preventParentAbort(int signal)
{
    //Le CTRL+C est traité par le rawmode dans le userinput.cpp,
    //mais cette fonction permet uniquement de ne pas terminer le shell lorsque que l'on
    //CTRL+C un fork
}

int main(int argc, char **argv)
{
    signal(SIGINT, &preventParentAbort);

    //On peut envisager plus tard que des sous shells se lance dans le premier le $(command) de bash par exemple
    Shell shell;
    shell.run();

    return 0;
}
