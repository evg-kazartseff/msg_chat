//
// Created by evgenii on 09.10.17.
//

#ifndef OS_NETWORK_CHAT_SERVER_TYPE_H
#define OS_NETWORK_CHAT_SERVER_TYPE_H

#define MAX_MSG_SIZE 512
#define MAX_DEST_NAME 80

typedef struct {
    long        mtype;
    char        buf[MAX_MSG_SIZE];
} Message_t;

typedef struct {
    long        mtype_send;
    long        mtype_recv;
    char        nane_client[MAX_DEST_NAME];
} client_t;

#endif //OS_NETWORK_CHAT_SERVER_TYPE_H
