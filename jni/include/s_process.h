#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "misc.h"
#include "list.h"

extern pthread_mutex_t rcv_list_mutex;
extern struct list_head rcv_list;
extern pthread_mutex_t snd_list_mutex;
extern struct list_head snd_list;

extern pthread_cond_t snd_cond;
extern pthread_cond_t rcv_cond;

extern int quit;

void *handle_snd_pkt(void *arg);


void create_snd_thread();


void *handle_rcv_pkt(void *arg);

void create_rcv_thread();
