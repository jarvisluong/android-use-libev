#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#include "misc.h"
#include "s_client.h"
#include "global.h"
#include "server.h"

int quit = 0;
ev_io exit_watcher;

ev_io socket_watcher;
ev_timer timeout_watcher;
ev_periodic periodic_watcher;

ev_async main_async_watcher;

struct list_head client_list;
pthread_mutex_t client_list_mutex;

int sockfd;
struct ev_loop *loop;

extern pthread_cond_t snd_cond;
extern pthread_cond_t rcv_cond;
extern pthread_mutex_t rcv_list_mutex;
extern pthread_mutex_t snd_list_mutex;

extern void on_msg_received(int code, char *msg);

void client_cb(struct ev_loop *loop, ev_io *w, int events);

void exit_main_thread()
{
	enter_func();

	close_all_client();
	close(sockfd);

	ev_break(loop, EVBREAK_ALL);
}

void close_all_client()
{
	Client *c;
	Client *tmp;

	enter_func();
	pthread_mutex_lock(&client_list_mutex);
	DBG(D_INFO, "%d clients \n", list_entry_count(&client_list));
	list_for_each_entry_safe(c, tmp, &client_list, list)
	{
		ev_io_stop(loop, &c->client_watcher);
		close(c->sockfd);
		list_del(&c->list);
		free(c);
	}
	pthread_mutex_unlock(&client_list_mutex);
}


void exit_all_thread()
{
	quit = 1;

	enter_func();

	//
    pthread_mutex_lock(&snd_list_mutex);
	pthread_cond_signal(&snd_cond);
    pthread_mutex_unlock(&snd_list_mutex);

    //
    pthread_mutex_lock(&rcv_list_mutex);
	pthread_cond_signal(&rcv_cond);
    pthread_mutex_unlock(&rcv_list_mutex);

	pthread_cond_destroy(&snd_cond);
	pthread_cond_destroy(&rcv_cond);
}


ev_tstamp scheduler(ev_periodic *w, ev_tstamp now)
{
	return (now + 5);
}


void dump_client_list()
{
	Client *c;

	enter_func();
	pthread_mutex_lock(&client_list_mutex);
	DBG(D_INFO, "%d clients\n", list_entry_count(&client_list));
	list_for_each_entry(c, &client_list, list)
	{
		DBG(D_INFO, "addr = %s\n", inet_ntoa(c->addr.sin_addr));
	}
	pthread_mutex_unlock(&client_list_mutex);
}


void handle_new_client(int clientfd, struct sockaddr_in addr)
{
	enter_func();
	Client *client = (Client *)malloc(sizeof(Client));
	client->sockfd = clientfd;
	client->addr = addr;

	pthread_mutex_init(&client->lock, NULL);
	INIT_LIST_HEAD(&client->list);

	pthread_mutex_lock(&client_list_mutex);
	list_add_tail(&client->list, &client_list);
	pthread_mutex_unlock(&client_list_mutex);

	ev_io_init(&client->client_watcher, client_cb, clientfd, EV_READ);

	ev_io_start(loop, &client->client_watcher);

	send(clientfd, "hello client \n", 16, 0);
	dump_client_list();
}


Client *get_client_by_fd(int fd)
{
	Client *c = NULL;

	pthread_mutex_lock(&client_list_mutex);
	list_for_each_entry(c, &client_list, list)
	{
		if (c->sockfd == fd) {
			break;
		}
	}
	pthread_mutex_unlock(&client_list_mutex);

	return (c);
}


void periodic_cb(struct ev_loop *loop, ev_io *w, int events)
{
	DBG(D_INFO, "periodic_cb\n");
	Client *c;

	on_msg_received(1, "on msg callback\n");

	pthread_mutex_lock(&client_list_mutex);
	DBG(D_INFO, "%d clients \n", list_entry_count(&client_list));
	list_for_each_entry(c, &client_list, list)
	{
        DBG(D_DEBUG, "client socket = %d\n", c->sockfd);
		int s = c->sockfd;

		send(s, "periodic\n", 10, 0);
	}
	pthread_mutex_unlock(&client_list_mutex);
}


void socket_cb(struct ev_loop *loop, ev_io *w, int events)
{
	int newfd;
	struct sockaddr_in cliaddr;
	int n;

	DBG(D_INFO, "got a new client\n");
	n = sizeof(struct sockaddr);
	newfd = accept(sockfd, (struct sockaddr *)&cliaddr, &n);
	if (newfd < 0) {
		perror("accept ");
		close(newfd);
		return;
	}

	handle_new_client(newfd, cliaddr);
}


void client_cb(struct ev_loop *loop, ev_io *w, int events)
{
	int clifd;
	char buffer[1024];
	Client *cli;
	int n_read;

	clifd = w->fd;
	cli = get_client_by_fd(clifd);

	n_read = read(clifd, buffer, sizeof(buffer));
	if (n_read <= 0) {
		DBG(D_INFO, "client closed\n");

		pthread_mutex_lock(&client_list_mutex);
		list_del(&cli->list);
		pthread_mutex_unlock(&client_list_mutex);

		ev_io_stop(loop, &cli->client_watcher);
		close(clifd);
		free(cli);
	} else {
		DBG(D_INFO, "server recv from %s, msg : %s\n", inet_ntoa(cli->addr.sin_addr), buffer);
		handle_readable_client(cli);
	}
}


void handle_readable_client(Client *client)
{
	int n_read;
	int buffer[1024];
	int clifd;

	enter_func();
}

void async_cb(struct ev_loop *loop, ev_async *w, int revent)
{
	enter_func();
	exit_main_thread();
}


int init_socket()
{
	int sockfd;
	int option = 1;
	int ret;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket ");
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(TCP_PORT);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
		perror("setsockopt ");
		return (-1);
	}

	ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (ret != 0) {
		perror("bind ");
		return (-1);
	}

	ret = listen(sockfd, 5);
	if (ret != 0) {
		perror("listen ");
		return (-1);
	}
	return (sockfd);
}


int init_socket_watcher()
{
	ev_io_init(&socket_watcher, socket_cb, sockfd, EV_READ);
	ev_io_start(loop, &socket_watcher);
	return (0);
}


int init_periodic_watcher()
{
	ev_periodic_init(&periodic_watcher, periodic_cb, 0, 0, scheduler);
	ev_periodic_start(loop, &periodic_watcher);
	return (0);
}

int init_async_watcher()
{
	ev_async_init(&main_async_watcher, async_cb);
	ev_async_start(loop, &main_async_watcher);
	return (0);
}

void init_client_list()
{
	INIT_LIST_HEAD(&client_list);
	pthread_mutex_init(&client_list_mutex, NULL);
}


int main()
{
	enter_func();
	loop = ev_default_loop(0);

	init_client_list();

	sockfd = init_socket();
	if (sockfd < 0) {
		DBG(D_ERROR, "init_socket failed \n ");
		exit(0);
	}
	init_socket_watcher();
	init_async_watcher();
	init_periodic_watcher();

    ev_run(loop, 0);

    ev_loop_destroy(loop);
    DBG(D_INFO, "ev_run exits");
	return (0);
}
