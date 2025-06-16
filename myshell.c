
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "myshell.h"

#define MAX_LINE 1024

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    char *args[64];
    char cwd[1024];
    FILE *input = stdin;

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror("Batch file not found");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        getcwd(cwd, sizeof(cwd));
        printf("%s> ", cwd);

        if (fgets(line, sizeof(line), input) == NULL) {
            break;
        }

        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;

        parseCommand(line, args);
        executeCommand(args);
    }

    if (input != stdin) fclose(input);
    return 0;
}
