#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>
#include "list.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <ev.h>

typedef struct {
    int     sockfd;
    struct sockaddr_in addr;
    char    username[MAX_STRING_LEN];
    char    serial[MAX_STRING_LEN];

    pthread_mutex_t lock;
    ev_io   client_watcher;

    struct list_head list;
    
}Client, *PClient;

#endif
