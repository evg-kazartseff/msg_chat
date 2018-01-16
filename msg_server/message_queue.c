//
// Created by evgenii on 09.10.17.
//

#include "message_queue.h"

MQueue_t* MQueue_Create() {
    MQueue_t* instanse = malloc(sizeof(MQueue_t));
    instanse->head = NULL;
    instanse->tail = NULL;
    instanse->number_of_lists = 0;
    return instanse;
}

int MQueue_Insert(MQueue_t* queue, Message_t* message) {
    if ((queue == NULL) || (message == NULL))
        return 1;
    if (queue->number_of_lists == queue->limit)
        return 2;
    list_t* new = malloc(sizeof(list_t));
    if (queue->head == NULL)
        queue->tail = new;
    new->message = message;
    new->next = queue->head;
    new->prev = NULL;
    if (queue->head != NULL)
        queue->head->prev = new;
    queue->head = new;
    queue->number_of_lists++;
    return 0;
}

int MQueue_Pop(MQueue_t* queue, Message_t** message) {
    if (queue == NULL)
        return 1;
    if ((queue->head == NULL) || (queue->tail == NULL))
        return 1;
    *message = queue->tail->message;
    if (queue->head->next == NULL)
        queue->head = NULL;
    if (queue->tail->prev != NULL)
        queue->tail->prev->next = NULL;
    list_t* tmp = queue->tail;
    queue->tail = queue->tail->prev;
    free(tmp);
    queue->number_of_lists--;
    return 0;
}

int MQueue_SetLimit(MQueue_t* queue, unsigned long limit) {
    queue->limit = limit;
    return 0;
}

long MQueue_Size(MQueue_t *queue) {
    if (queue == NULL)
        return -1;
    return queue->number_of_lists;
}
