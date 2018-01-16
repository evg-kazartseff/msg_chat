//
// Created by evgenii on 15.10.17.
//

#ifndef OS_NETWORK_CHAT_CLIENT_CLIENT_H
#define OS_NETWORK_CHAT_CLIENT_CLIENT_H

#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <zconf.h>
#include <fcntl.h>
#include <semaphore.h>
#include "msg_t.h"

typedef struct {
    int msg_d;
    key_t key;
    int runing;
    pthread_mutex_t runing_mutex;
    pthread_t pthread[2];
    sem_t* semaphore;
    long mtype_send;
    pthread_mutex_t mtype_send_mutex;
    long mtype_recv;
    pthread_mutex_t mtype_recv_mutex;
    char* name_dest;
    pthread_mutex_t name_dest_mutex;
    pthread_mutex_t print_mutex;
} Client_t;

int ClientRun(char* arg1, char arg2);
int ClientWait();
int ClientFinalize();
void* ClientSender(void*);
void* ClientReciver(void*);


char *read_pipe();

#endif //OS_NETWORK_CHAT_CLIENT_CLIENT_H

