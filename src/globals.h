#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>
#include <unistd.h>

extern const char *uid;
extern const char *pwd;
extern char cwd[255];
extern pid_t cid;
extern pthread_mutex_t lock;

#endif