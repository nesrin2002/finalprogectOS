
#ifndef MYSHELL_H
#define MYSHELL_H

void parseCommand(char *line, char **args);
void executeCommand(char **args);
void handleRedirection(char **args, int *background);

#endif
