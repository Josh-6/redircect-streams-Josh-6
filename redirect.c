#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void break_into_words(char *cmd, char *child_arguments[], char break_char)
{
    int current_argument = 0;
    char current_word[1000];
    current_word[0] = '\0';
    char *current_letter = cmd;
    while (*current_letter != '\0')
    {
        // If current letter is a space then add the current word to the array and get ready for the next word
        if (*current_letter == break_char)
        {
            // Copy the new word into the child arguments
            child_arguments[current_argument] = strdup(current_word);
            // The next place we'll put the argument
            current_argument++;
            // Prepare for the next argument
            current_word[0] = '\0';
        }
        else
        {

            current_word[strlen(current_word) + 1] = '\0';
            current_word[strlen(current_word)] = *current_letter;
        }
        *current_letter++;
    }
    
    child_arguments[current_argument] = strdup(current_word);
    
    current_argument++;

    child_arguments[current_argument] = NULL;
}

void find_absolute_path(char *command, char *absolute_path_to_command)
{
    strcpy(absolute_path_to_command, command);
    char* path = getenv("PATH");

    char* directory = strtok(path, ":");

    while (directory != NULL)
    {

        directory = strtok(NULL, ":");
    }

    if (path != NULL) {
        printf("PATH: %s\n", path);
    } else {
        printf("PATH environment variable not found.\n");
    }

    // Get the environmental path "PATH"
    // Break it into words on ":"
    // Go through each entry
    // See if that entry + command exists as a file
    // If it does copy into absolute_path_to_command
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "redir <inp> <cmd> <out>\n");
        exit(1);
    }

    char *cmd = argv[2];
    char absolute_path_to_command[1000];
    char *child_arguments[20];

    break_into_words(cmd, child_arguments, ' ');
    for (char **argument_pointer = child_arguments; *argument_pointer != NULL; *argument_pointer++)
    {
        printf("%s\n", *argument_pointer);
    }

    find_absolute_path(child_arguments[0], absolute_path_to_command);

    
    int input_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (input_fd == -1)
    {
        fprintf(stderr, "Failed to open %s\n", argv[3]);
        return 1;
    }

    
    int output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1)
    {
        fprintf(stderr, "Failed to open %s\n", argv[3]);
        return 1;
    }
    
    int pid = fork();
    if (pid == 0)
    {
        // Stdin = input_fd
        dup2(input_fd, STDOUT_FILENO); 
        close(input_fd);

        // Stdout = output_fd
        dup2(output_fd, STDOUT_FILENO); 
        close(output_fd);

        printf("I am the child about to execute absolute command\n");
        execve(absolute_path_to_command, child_arguments, NULL);
        printf("Unable to execute: %s", absolute_path_to_command);
        _exit(1);
    }
    else {
        wait(NULL);
    }

    for (int i = 0; child_arguments[i] != NULL; i++) {
        free(child_arguments[i]); // Free each duplicated string
    }

    return 0;
}
