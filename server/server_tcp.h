#ifndef _SERVER_TCP_H_
#define _SERVER_TCP_H_

#include "base.h"

// #define SERVER_IP   "192.168.31.232"
#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT_NUM    8000
#define MAX_SOCKET_QUEUE_LENGTH 128
#define MAX_EVENT_SIZE  1024

typedef struct server_tcp_event {
    int32_t fd;
    int32_t events;
    void (*call_back)(int32_t fd, int32_t events, void *arg);
    void *arg;
    char buf[1024];
    int32_t buflen;
    int32_t epfd;
} sevent;

extern sevent my_events[MAX_EVENT_SIZE + 1];

int32_t server_tcp_init(void);
int32_t server_tcp_epoll(void (*handle_buf)(char*, int32_t*));

#endif  // _SERVER_TCP_H_