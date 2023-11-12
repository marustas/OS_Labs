#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>

#define MAX_TEXT 512

struct message
{
    long msg_type;
    char msg_text[MAX_TEXT];
};

int countWords(char *str)
{
    int state = 0;
    unsigned count = 0;

    while (*str)
    {

        if (*str == ' ' || *str == '\n' || *str == '\t')
        {
            state = 0;
        }

        else if (state == 0)
        {
            state = 1;
            ++count;
        }
        ++str;
    }

    return count;
}

int main()
{
    int msgid;
    pid_t pid;
    char *filename = "Test.txt";

    // Create a message queue
    msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget failed");
        return EXIT_FAILURE;
    }

    // Fork the process
    pid = fork();
    if (pid == -1)
    {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid > 0)
    {
        // Parent process
        struct message msg_to_receive;

        // Receive a message
        if (msgrcv(msgid, &msg_to_receive, sizeof(msg_to_receive), 1, 0) == -1)
        {
            perror("msgrcv failed");
            return EXIT_FAILURE;
        }

        printf("Received in parent: %s\n", msg_to_receive.msg_text);
        printf("Word count: %d\n", countWords(msg_to_receive.msg_text));

        // Destroy the message queue
        if (msgctl(msgid, IPC_RMID, NULL) == -1)
        {
            perror("msgctl(IPC_RMID) failed");
            return EXIT_FAILURE;
        }
    }
    else
    {
        // Child process
        struct message msg_to_send;
        FILE *file;
        size_t bytesRead;

        file = fopen(filename, "r");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        // Read file content
        bytesRead = fread(msg_to_send.msg_text, 1, MAX_TEXT - 1, file);
        msg_to_send.msg_text[bytesRead] = '\0';

        fclose(file);

        msg_to_send.msg_type = 1;

        // Send message
        if (msgsnd(msgid, &msg_to_send, strlen(msg_to_send.msg_text) + 1, 0) == -1)
        {
            perror("msgsnd failed");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }

    return EXIT_SUCCESS;
}