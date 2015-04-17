#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "ev.h"

#include "global.h"
#include "misc.h"

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"

static ev_io socket_watcher;
ev_async bc_async_watcher;
static ev_periodic periodic_watcher;

static int sockfd;
struct ev_loop *bc_loop;
static int ret;

void exit_bc_thread()
{
	enter_func();
	close(sockfd);

	ev_break(bc_loop, EVBREAK_ALL);
}


ev_tstamp bc_scheduler(ev_periodic *w, ev_tstamp now)
{
	return (now + 5);
}

void bc_periodic_cb(struct ev_loop *bc_loop, ev_io *w, int events)
{
	DBG(D_INFO, "bcast prriodic_cb\n");
}


void bc_socket_cb(struct ev_loop *bc_loop, ev_io *w, int events)
{
	int count;
	struct sockaddr_in client_addr;
	int addr_len;
	char buffer[1024];

	count = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_len);
	if (strstr(buffer, IP_FOUND)) {
		DBG(D_INFO, "Client connection information:\n\t IP: %s, Port: %d\n",
				inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK)+1);
		count = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&client_addr, addr_len);
	}
}

void bc_async_cb(struct ev_loop *bc_loop, ev_async *w, int revent)
{
	enter_func();
	exit_bc_thread();
}


int init_bc_socket()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in server_addr;
	int option = 1;

	if (sockfd < 0) {
		perror("sock error\n");
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
		perror("setsockopt ");
		return (-1);
	}
	int addr_len = sizeof(struct sockaddr_in);

	memset((void*)&server_addr, 0, addr_len);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(UDP_BCAST_PORT);

	ret = bind(sockfd, (struct sockaddr*)&server_addr, addr_len);
	if (ret < 0) {
		perror("bind error\n");
		return -1;
	}
	return (sockfd);
}


int init_bc_socket_watcher()
{
	ev_io_init(&socket_watcher, bc_socket_cb, sockfd, EV_READ);
	ev_io_start(bc_loop, &socket_watcher);
	return (0);
}

int init_bc_periodic_watcher()
{
	ev_periodic_init(&periodic_watcher, bc_periodic_cb, 0, 0, bc_scheduler);
	ev_periodic_start(bc_loop, &periodic_watcher);
	return (0);
}

int init_bc_async_watcher()
{
	ev_async_init(&bc_async_watcher, bc_async_cb);
	ev_async_start(bc_loop, &bc_async_watcher);
	return (0);
}


int bc_main()
{
	enter_func();
	bc_loop = ev_loop_new(0);

	sockfd = init_bc_socket();
	if (sockfd < 0) {
		DBG(D_ERROR, "init_socket failed \n ");
		exit(0);
	}
	init_bc_socket_watcher();
	init_bc_async_watcher();
	init_bc_periodic_watcher();

    ev_run(bc_loop, 0);

    ev_loop_destroy(bc_loop);
    DBG(D_INFO, "bcast ev_run exits");
	return (0);
}
