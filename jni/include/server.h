#include <ev.h>

#include "misc.h"
#include "s_client.h"
#include "global.h"

extern struct list_head client_list;
extern pthread_mutex_t client_list_mutex;
extern ev_async main_async_watcher;
extern struct ev_loop *loop;

void exit_main_thread();


