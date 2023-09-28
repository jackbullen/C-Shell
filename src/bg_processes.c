#include "bg_processes.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

struct Node *head = NULL;

struct Node* getHead() {
    return head;
}

struct Node *createNode(struct bgProcess data) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
    if (!newNode) {
        perror("Unable to allocate memory for new node");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

void addProcess(struct bgProcess data) {
    struct Node *newNode = createNode(data);
    newNode->next = head;
    newNode->data.name = strdup(data.name);
    head = newNode;
}

int removeProcess(pid_t pid) {
    struct Node *current = head, *prev = NULL;
    while (current != NULL) {
        if (current->data.pid == pid) {
            if (prev == NULL) {
                head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->data.name);
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

void cleanUp() {
    struct Node *current = head, *next = NULL;
    while (current != NULL) {
        next = current->next;

        if (kill(current->data.pid, SIGTERM) == -1){
            if (errno == ESRCH) {
                printf("Process %d not found\n", current->data.pid);
            } else {
                perror("Error killing process");
            }
        }
        free(current->data.name);
        free(current);
        current = next;
    }
}