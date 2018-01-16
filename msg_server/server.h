//
// Created by evgenii on 03.10.17.
//

#ifndef OS_NETWORK_CHAT_SERVER_H
#define OS_NETWORK_CHAT_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <zconf.h>
#include <semaphore.h>
#include <fcntl.h>

#include "server_type.h"
#include "message_queue.h"

#define PERM 0666
#define MAX_CLIENT 256
#define MAX_SIZE_QUEUE 500

typedef struct {
    key_t           key;
    int             msg_d;
    int             runing;
    pthread_mutex_t runing_mutex;
    sem_t*          semaphore;
    char            sem_name[16];
    client_t        clients[MAX_CLIENT];
    unsigned long   engaged_client;
    pthread_mutex_t clients_mutex;
    MQueue_t*       MQueue;
    pthread_mutex_t MQueue_mutex;
    unsigned long   engaged_mtype;
} Server_t;

int ServerRun(const char* arg1, char arg2);

void* ServerOpenConnection(void *);
void* ServerReciver(void *);
void* ServerSender(void *);
void* ServerSendNewClient(void *);
int ServerFinalize();

#endif //OS_NETWORK_CHAT_SERVER_H
