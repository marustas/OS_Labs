#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>

#define MAX_TEXT 512
#define FILE_NAME "text.txt"

struct message_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
} message;

//Function for counting words
int countWords(char *str) {
    int state = 0;
    unsigned count = 0;

    while (*str) {
        if (*str == ' ' || *str == '\n' || *str == '\t') {
            state = 0;
        }
        else if (state == 0) {
            state = 1;
            count++;
        }
        str++;
    }

    return count;
}

int main() {
    int msgid;
    pid_t pid;

    // Create a message queue with a unique id and exit if an error occurs
    msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Failed to create Message Queue");
        exit(EXIT_FAILURE);
    }

    // Fork the process and exit if an error occurs
    pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process
        /* 
        Receive the message with the defined max sixe in bytes
        and exit if an error occurs.
        Message type is 1, 
        but it can be any number as long as we can keep track of the messages.
        */
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1) {
            perror("Failed to receive the message");
            exit(EXIT_FAILURE);
        }

        printf("Received in parent: %s\n", message.msg_text);
        printf("Number of words in %s: %d\n", FILE_NAME, countWords(message.msg_text));

        // Destroy the message queue and exit if an error occurs
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl(IPC_RMID) failed");
            return EXIT_FAILURE;
        }
    } else {
        // Child process
        FILE *file;
        size_t bytesRead;

        //Open the file and exit if an error occurs
        file = fopen(FILE_NAME, "r");
        if (file == NULL) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        // Read the content of the file, each element is 1 byte, read into the message
        bytesRead = fread(message.msg_text, 1, MAX_TEXT - 1, file);
        message.msg_text[bytesRead] = '\0';

        fclose(file);

        message.msg_type = 1;

        /* 
        Send the message with the size equal to the length.
        Message type is 1, same as in Parent process
        Exit if an error occurs
        */
        if (msgsnd(msgid, &message, strlen(message.msg_text), 0) == -1) {
            perror("Failed to send the message");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    return 0;
}