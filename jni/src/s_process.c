#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#include "misc.h"
#include "list.h"

pthread_mutex_t rcv_list_mutex;
struct list_head rcv_list;
pthread_mutex_t snd_list_mutex;
struct list_head snd_list;

pthread_cond_t snd_cond;
pthread_cond_t rcv_cond;

extern int quit;

void *handle_snd_pkt(void *arg)
{
	enter_func();
	int i = 0;

	while (!quit) {
		pthread_mutex_lock(&snd_list_mutex);
		while (list_entry_count(&snd_list) == 0 && quit != 1) {
			DBG(D_INFO, "handle_snd_pkt wait\n");
			pthread_cond_wait(&snd_cond, &snd_list_mutex);
		}
		DBG(D_INFO, "do something in handle_snd_pkt\n");
		//do something of the list

		if (quit) {
			pthread_mutex_unlock(&snd_list_mutex);
			DBG(D_INFO, "thread handle_snd_pkt exit\n");
			return (NULL);
		}


		pthread_mutex_unlock(&snd_list_mutex);
	}

	return (NULL);
}

void *handle_rcv_pkt(void *arg)
{
	enter_func();
	while (!quit) {
		pthread_mutex_lock(&rcv_list_mutex);
		while (list_entry_count(&rcv_list) == 0 && quit != 1) {
            DBG(D_INFO, "handle_snd_pkt wait\n");
			pthread_cond_wait(&rcv_cond, &rcv_list_mutex);
		}
		DBG(D_INFO, "do something in handle_rcv_pkt\n");
		//do something of the list

		if (quit) {
			pthread_mutex_unlock(&rcv_list_mutex);
			DBG(D_INFO, "thread handle_rcv_pkt exit\n");
			return (NULL);
		}
		//do something of the list



		pthread_mutex_unlock(&rcv_list_mutex);
	}

	return (NULL);
}

