

#include "includes.hpp"

void fatal_error(char *message, char *file, int line)
{
        char errbuf[512];
        if (file) {
                sprintf(errbuf, "Error (%s line %d): %s\n", file, line, message);
                message = errbuf;
        }
        MessageBox(NULL, message, "e r r o r !", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
        ExitProcess(1);
}

void fatal_error(char *message)
{
	MessageBox(NULL, message, "e r r o r !", MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
        ExitProcess(1);
}