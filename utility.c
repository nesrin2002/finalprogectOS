
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "myshell.h"

void parseCommand(char *line, char **args) {
    int i = 0;
    args[i] = strtok(line, " \t");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t");
    }
}

void handleRedirection(char **args, int *background) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            *background = 1;
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0) {
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) perror("Failed to open output file");
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0) {
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) perror("Failed to open output file");
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
        }
        else if (strcmp(args[i], "<") == 0) {
            int fd = open(args[i+1], O_RDONLY);
            if (fd < 0) perror("Failed to open input file");
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
        }
    }
}

void executeCommand(char **args) {
    if (args[0] == NULL) return;

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("Current Directory: %s\n", cwd);
        } else {
            if (chdir(args[1]) != 0)
                perror("cd error");
            else {
                char cwd[1024];
                getcwd(cwd, sizeof(cwd));
                setenv("PWD", cwd, 1);
            }
        }
    }
    else if (strcmp(args[0], "clr") == 0) {
        system("clear");
    }
    else if (strcmp(args[0], "dir") == 0) {
        char cmd[1024] = "ls -al ";
        if (args[1]) strcat(cmd, args[1]);
        else strcat(cmd, ".");
        system(cmd);
    }
    else if (strcmp(args[0], "environ") == 0) {
        extern char **environ;
        for (char **env = environ; *env; env++)
            printf("%s\n", *env);
    }
    else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i]; i++)
            printf("%s ", args[i]);
        printf("\n");
    }
    else if (strcmp(args[0], "pause") == 0) {
        printf("Press Enter to continue...");
        getchar();
    }
    else if (strcmp(args[0], "help") == 0) {
        system("more readme");
    }
    else if (strcmp(args[0], "quit") == 0) {
        exit(0);
    }
    else {
        int background = 0;
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
        }
        else if (pid == 0) {
            handleRedirection(args, &background);
            execvp(args[0], args);
            perror("Command failed");
            exit(EXIT_FAILURE);
        }
        else {
            if (!background) {
                waitpid(pid, NULL, 0);
            }
        }
    }
}
