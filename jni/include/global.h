#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "list.h"

#define     TCP_PORT            9999
#define 	UDP_BCAST_PORT		9998

#define     MAX_HEADER_DATA_LENGTH     1024  // 1k

#define     RECONNECT_TIME_OUT    5   //seconds

typedef enum {
    NORMAL,
} CMD;

typedef struct _MSG_PKT_HDR {
    int     h_body_type;
    int     h_body_size;
    int     h_code;
    int     h_cmd;

    struct list_head list;  
     
    char    data[MAX_HEADER_DATA_LENGTH];
     
}__attribute__ (( packed )) MSG_PKT_HDR, *PMSG_PKT_HDR;

#endif
