//
// Created by evgenii on 03.10.17.
//
#include "server.h"

Server_t* Server;

int ServerRun(const char *arg1, char arg2) {
    if (Server == NULL) {
        Server = malloc(sizeof(Server_t));
        Server->runing = 1;
        Server->engaged_mtype = 3;
        Server->engaged_client = 0;
        if ((Server->key = ftok(arg1, arg2)) < 0) {
            fprintf(stderr, "Server: ftok error\n");
            return 1;
        }
        if ((Server->semaphore = sem_open("/chatr", O_CREAT, 0777, 1)) == SEM_FAILED) {
            fprintf(stderr, "Server: sem_open error\n");
            return 1;
        }
        sem_wait(Server->semaphore);
        if ((Server->msg_d = msgget(Server->key, PERM | IPC_CREAT)) < 0) {
            sem_post(Server->semaphore);
            fprintf(stderr, "Server: msgget error\n");
            return 1;
        }
        sem_post(Server->semaphore);
        pthread_mutex_lock(&Server->MQueue_mutex);
        Server->MQueue = MQueue_Create();
        MQueue_SetLimit(Server->MQueue, MAX_SIZE_QUEUE);
        pthread_mutex_unlock(&Server->MQueue_mutex);
        pthread_t pthread;
        if(pthread_create(&pthread, NULL, ServerOpenConnection, NULL)) {
            fprintf(stderr, "Server: Error creating thread\n");
            return 1;
        }
        pthread_detach(pthread);
        if(pthread_create(&pthread, NULL, ServerSender, NULL)) {
            fprintf(stderr, "Server: Error creating thread\n");
            return 1;
        }
        pthread_detach(pthread);
        printf("Server: Server has been started!\n");
    }
    return 0;
}

void* ServerOpenConnection(void *arg) {
    pthread_mutex_lock(&Server->runing_mutex);
    while (Server->runing) {
        pthread_mutex_unlock(&Server->runing_mutex);
        Message_t message;
        message.mtype = 1L;
        sem_wait(Server->semaphore);
        ssize_t n = msgrcv(Server->msg_d, &message, sizeof(Message_t), message.mtype, IPC_NOWAIT);
        sem_post(Server->semaphore);
        if (n > 0) {
            client_t client;
            strcpy(client.nane_client, message.buf);
            client.mtype_send = Server->engaged_mtype;
            client.mtype_recv = Server->engaged_mtype + 1;
            Server->engaged_mtype++;
            sprintf(message.buf, "%ld:%ld", client.mtype_send, client.mtype_recv);
            message.mtype = 2L;
            Server->engaged_mtype++;
            sem_wait(Server->semaphore);
            msgsnd(Server->msg_d, &message, sizeof(Message_t), 0);
            sem_post(Server->semaphore);
            pthread_mutex_lock(&Server->clients_mutex);
            Server->clients[Server->engaged_client] = client;
            Server->engaged_client++;
            pthread_mutex_unlock(&Server->clients_mutex);
            pthread_t pthread;
            if(pthread_create(&pthread, NULL, ServerReciver, (void *) (client.mtype_recv))) {
                fprintf(stderr, "Server: Error creating thread\n");
                exit(1);
            }
            pthread_detach(pthread);
            if(pthread_create(&pthread, NULL, ServerSendNewClient, (void *) (client.nane_client))) {
                fprintf(stderr, "Server: Error creating thread\n");
                exit(1);
            }
            pthread_detach(pthread);
            dprintf(STDOUT_FILENO, "Client '%s' connected!\n", client.nane_client);
        }
        usleep(1000000);
        pthread_mutex_lock(&Server->runing_mutex);
    }
    return 0;
}

int ServerFinalize() {
    if (Server != NULL) {
        Server->runing = 0;
        sem_wait(Server->semaphore);
        if (msgctl(Server->msg_d, IPC_RMID, 0) < 0) {
            sem_post(Server->semaphore);
            fprintf(stderr, "msgctl error\n");
            exit(EXIT_FAILURE);
        }
        sem_post(Server->semaphore);
        sem_close(Server->semaphore);
        free(Server);
    }
    return 0;
}

void *ServerReciver(void *arg) {
    pthread_mutex_lock(&Server->runing_mutex);
    while (Server->runing) {
        pthread_mutex_unlock(&Server->runing_mutex);
        Message_t message;
        message.mtype = (long) arg;
        sem_wait(Server->semaphore);
        ssize_t n = msgrcv(Server->msg_d, &message, sizeof(Message_t), message.mtype, IPC_NOWAIT);
        sem_post(Server->semaphore);
        if (n > 0) {
            Message_t *mess = malloc(sizeof(Message_t));
            char buff_name[512];
            char my_name[512];
            strcpy(buff_name, message.buf);
            char *p = strtok(buff_name, ":");
            for (int i = 0; i < MAX_CLIENT; ++i) {
                if (strcmp(p, Server->clients[i].nane_client) == 0) {
                    mess->mtype = Server->clients[i].mtype_send;
                }
                if (message.mtype == Server->clients[i].mtype_recv) {
                    strcpy(my_name, Server->clients[i].nane_client);
                }
            }
            p = strtok(NULL, ":");
            strcpy(mess->buf, my_name);
            strcat(mess->buf, ":");
            strcat(mess->buf, p);
            MQueue_Insert(Server->MQueue, mess);
        }
        usleep(500000);
        pthread_mutex_lock(&Server->runing_mutex);
    }
}

void *ServerSender(void *arg) {
    pthread_mutex_lock(&Server->runing_mutex);
    while (Server->runing) {
        pthread_mutex_unlock(&Server->runing_mutex);
        if (MQueue_Size(Server->MQueue) > 0) {
            Message_t* message;
            MQueue_Pop(Server->MQueue, &message);
            sem_wait(Server->semaphore);
            msgsnd(Server->msg_d, message, sizeof(Message_t), IPC_NOWAIT);
            sem_post(Server->semaphore);
            free(message);
        }
        usleep(500000);
        pthread_mutex_lock(&Server->runing_mutex);
    }
}

void *ServerSendNewClient(void * name) {
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if ((Server->clients[i].mtype_send != 0) && (strcmp(Server->clients[i].nane_client, name) != 0)) {
            Message_t message;
            message.mtype = Server->clients[i].mtype_send;
            sprintf(message.buf, "Client '%s' connected!", (char *) (name));
            sem_wait(Server->semaphore);
            if (msgsnd(Server->msg_d, &message, sizeof(Message_t), 0) < 0) {
                sem_post(Server->semaphore);
                fprintf(stderr, "Client: Send error\n");
                exit(EXIT_FAILURE);
            }
            sem_post(Server->semaphore);
        }
    }
}


