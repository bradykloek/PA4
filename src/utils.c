#include "utils.h"

void _removeOutputDir()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0)
        {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}

void _createOutputDir()
{
    mkdir("output", ACCESSPERMS);
}

void bookeepingCode()
{
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
}

// --- BONUS: Caesar shift-right-by-2 cipher ---
// Used to lightly obfuscate the query sent by ENC_SEARCH_ITEM.
// Shifts letters only; non-letters are passthrough.
// Wraps at the end of the alphabet: 'y'->'a', 'z'->'b', 'Y'->'A', 'Z'->'B'.
void encrypt_str(char *s)
{
    //TODO
}

void decrypt_str(char *s)
{
    //TODO
}
