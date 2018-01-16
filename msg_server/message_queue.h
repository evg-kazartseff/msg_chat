//
// Created by evgenii on 09.10.17.
//

#ifndef OS_NETWORK_CHAT_MESSAGE_QUEUE_H
#define OS_NETWORK_CHAT_MESSAGE_QUEUE_H

#include "server_type.h"
#include <stdlib.h>

typedef struct list list_t;

struct list {
    Message_t* message;
    list_t* next;
    list_t* prev;
};

struct message_queue {
    list_t* head;
    list_t* tail;
    unsigned long limit;
    unsigned long number_of_lists;
};

typedef struct message_queue MQueue_t;

MQueue_t* MQueue_Create();
int MQueue_SetLimit(MQueue_t* queue, unsigned long limit);
int MQueue_Insert(MQueue_t* queue, Message_t* message);
int MQueue_Pop(MQueue_t* queue, Message_t** message);
long MQueue_Size(MQueue_t *queue);

#endif //OS_NETWORK_CHAT_MESSAGE_QUEUE_H
