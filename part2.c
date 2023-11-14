#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>

#define MAX_TEXT 512

struct message_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
} message;

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
    char *filename = "Test.txt";

    // Create a message queue
    msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("Failedd to create Message Queue");
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid = fork();
    if (pid == -1) {
        perror("Failed to fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process

        // Receive the message
        if (msgrcv(msgid, &message, sizeof(message), 1, 0) == -1) {
            perror("Failed to receive the message");
            exit(EXIT_FAILURE);
        }

        printf("Received in parent: %s\n", message.msg_text);
        printf("Word count: %d\n", countWords(message.msg_text));

        // Destroy the message queue
        if (msgctl(msgid, IPC_RMID, NULL) == -1) {
            perror("msgctl(IPC_RMID) failed");
            return EXIT_FAILURE;
        }
    } else {
        // Child process
        FILE *file;
        size_t bytesRead;

        //Open the file
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Failed to open file");
            exit(EXIT_FAILURE);
        }

        // Read the content of the file, each element is 1 byte, read into the message
        bytesRead = fread(message.msg_text, 1, MAX_TEXT - 1, file);
        message.msg_text[bytesRead] = '\0';

        fclose(file);

        message.msg_type = 1;

        // Send the message
        if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
            perror("Failed to send the message");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    return 0;
}