#ifndef BG_PROCESSES_H
#define BG_PROCESSES_H

#include <stdlib.h>
#include <sys/types.h>

struct bgProcess {
    pid_t pid;
    char *name;
    int status;
    char *state;
};

struct Node {
    struct bgProcess data;
    struct Node *next;
};

struct Node* getHead(void);
struct Node *createNode(struct bgProcess data);
void addProcess(struct bgProcess data);
int removeProcess(pid_t pid);
void printList();
void printProcess(struct bgProcess data);

#endif