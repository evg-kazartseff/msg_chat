//
// Created by evgenii on 15.10.17.
//

#include "client.h"

Client_t* Client;

int ClientRun(char *arg1, char arg2) {
    if (Client == NULL) {
        Client = malloc(sizeof(Client));
        Client->runing = 1;
        Client->mtype_send = -1;
        Client->mtype_recv = -1;
        Client->name_dest = malloc(512 * sizeof(char));
        if ((Client->key = ftok(arg1, arg2)) < 0) {
            fprintf(stderr, "Client: ftok error\n");
            exit(EXIT_FAILURE);
        }
        if ((Client->semaphore = sem_open("/chatr", O_CREAT, 0777, 1)) == SEM_FAILED) {
            fprintf(stderr, "Client: sem_open error\n");
            return 1;
        }
        sem_wait(Client->semaphore);
        if ((Client->msg_d = msgget(Client->key, 0)) < 0) {
            sem_post(Client->semaphore);
            fprintf(stderr, "Client: msgget error\n");
            exit(EXIT_FAILURE);
        }
        sem_post(Client->semaphore);
        message_t msg;
        msg.mtype = 1L;
        printf("Please enter your name: ");
        strcpy(msg.buf, read_pipe());
        sem_wait(Client->semaphore);
        if (msgsnd(Client->msg_d, &msg, sizeof(message_t), 0) < 0) {
            sem_post(Client->semaphore);
            fprintf(stderr, "Client: msgsnd error\n");
            exit(EXIT_FAILURE);
        }
        sem_post(Client->semaphore);
        msg.mtype = 2L;
        ssize_t n;
        do {
            sem_wait(Client->semaphore);
            n = msgrcv(Client->msg_d, &msg, sizeof(message_t), msg.mtype, IPC_NOWAIT);
            sem_post(Client->semaphore);
        } while (n == -1);
        if (n > 0) {
            char *ptr;
            Client->mtype_recv = strtol(msg.buf, &ptr, 10);
            Client->mtype_send = strtol((ptr + 1), NULL, 10);
        }
        pthread_create(&(Client->pthread[0]), NULL, ClientSender, (void *) Client->mtype_send);
        pthread_create(&(Client->pthread[1]), NULL, ClientReciver, (void *) Client->mtype_recv);
    }
    return 0;
}

int ClientFinalize() {
    if (Client != NULL) {
        Client->runing = 0;
        sleep(2);
        free(Client->name_dest);

        free(Client);
    }
    return 0;
}

void *ClientSender(void* arg) {
    pthread_mutex_lock(&Client->print_mutex);
    dprintf(STDOUT_FILENO, "Write \"\\quit\" for exit!\n");
    dprintf(STDOUT_FILENO, "Who do you want to communicate with?\nEnter name: ");
    strcpy(Client->name_dest, read_pipe());
    pthread_mutex_unlock(&Client->print_mutex);
    while(Client->runing) {
        message_t message;
        message.mtype = (long) arg;
        char pipe[512];
        pthread_mutex_lock(&Client->print_mutex);
        dprintf(STDOUT_FILENO, "You: ");
        pthread_mutex_unlock(&Client->print_mutex);
        strcpy(pipe, read_pipe());
        strcpy(message.buf, Client->name_dest);
        strcat(message.buf, ": ");
        strcat(message.buf, pipe);
        if(!strcmp(pipe, "\\quit")) {
            Client->runing = 0;
            break;
        }
        else {
            sem_wait(Client->semaphore);
            if (msgsnd(Client->msg_d, &message, sizeof(message), 0) < 0) {
                sem_post(Client->semaphore);
                fprintf(stderr, "Client: Send error\n");
                exit(EXIT_FAILURE);
            }
            sem_post(Client->semaphore);
        }
    }
}

int ClientWait() {
    pthread_join(Client->pthread[1], NULL);
    pthread_join(Client->pthread[0], NULL);
    return 0;
}

void *ClientReciver(void *arg) {
    while(Client->runing) {
        message_t message;
        message.mtype = (long) arg;
        sem_wait(Client->semaphore);
        ssize_t n = msgrcv(Client->msg_d, &message, sizeof(message_t), message.mtype, IPC_NOWAIT);
        sem_post(Client->semaphore);
        if (n > 0) {
            pthread_mutex_lock(&Client->print_mutex);
            dprintf(STDOUT_FILENO, "\n%s\n", message.buf);
            pthread_mutex_unlock(&Client->print_mutex);
        }
        usleep(500000);
    }
}

char *read_pipe()
{
    size_t buf = 64;
    size_t readed = 0;
    char *output = calloc(buf, sizeof(char));
    do {
        if (readed == buf) {
            buf += 64;
            output = realloc(output, buf);
        }
        output[readed] = (char) getchar();
        readed++;
    } while (output[readed - 1] != '\n');
    output[readed - 1] = '\000';
    return output;
}