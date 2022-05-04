// Shoval Weinstock 209540731

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

char* commandsHistory[100];
int pidHistory[100];
int numOfCommands = 0;

void changePath(int argc, char *argv[]) {
    char* path = getenv("PATH");
    for (int i = 1; i < argc; i++) {
        strcat(path, ":");
        strcat(path, argv[i]);
    }
    setenv("PATH", path, 1);
}

void cd(char* subDir) {
    char* newDir = getenv("PWD");
    strcat(newDir,"/");
    strcat(newDir,subDir);
    int result = chdir(newDir);
    if (result != 0) {
        perror("chdir failed");
    }
}

void printHistory() {
    for (int i = 0; i < numOfCommands; i++) {
        printf("%d %s\n", pidHistory[i], commandsHistory[i]);
    }
}

void addToHistory(int pid, char* fullCommand) {
    pidHistory[numOfCommands] = pid;
    commandsHistory[numOfCommands] = (char*) malloc(sizeof(fullCommand));
    strcpy(commandsHistory[numOfCommands], fullCommand);
    numOfCommands++;
}

int main(int argc, char *argv[]) {

    changePath(argc, argv);

    char input[100];

    while(1) {
        // get input from the user
        printf("$ ");
        fflush(stdout);
        scanf("%[^\n]%*c", input);

        // separate the input to the command and its arguments
        char inputCopy[100];
        strcpy(inputCopy, input);
        char *command = strtok(inputCopy, " ");
        char *arguments = strtok(NULL, " ");

        // execute the command

        if (strcmp(command, "exit") == 0) {
            // free allocated memory before exit
            for (int i = 0; i < numOfCommands; i++) {
                free(commandsHistory[i]);
            }
            exit(0);
        }

        else if (strcmp(command, "cd") == 0) {
            cd(arguments);
            // add the command to the commandsHistory
            addToHistory(getpid(), input);
        }

        else if (strcmp(command, "history") == 0) {
            // add the command to the commandsHistory
            addToHistory(getpid(), input);
            // print the commandsHistory
            printHistory();
            continue;
        }

        else {
            int stat,waited,ret_code;
            pid_t pid;
            pid = fork();
            // if fork failed
            if( pid < 0) {
                perror("fork failed");
            }
            // child
            if (pid == 0) {
                char* commandAndArgs[100];
                commandAndArgs[0] = command;
                int j = 1;
                // extract arguments
                while( arguments != NULL ) {
                    commandAndArgs[j] = arguments;
                    arguments = strtok(NULL, " ");
                    j++;
                }
                commandAndArgs[j] = NULL;
                ret_code = execvp(command , commandAndArgs);
                if (ret_code == -1) {
                    perror("execvp failed");
                }
            }
                // parent
            else {
                addToHistory(pid, input);
                waited = wait(&stat);   /* stat can tell what happened */
            }
        }
    }
}