#ifndef _CLIENT_TCP_H_
#define _CLIENT_TCP_H_

#include "base.h"

// #define SERVER_IP   "192.168.31.232"
#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT_NUM    8000

int32_t client_tcp_init(void);
int32_t client_tcp_send_from_stdin(void);
void client_tcp_close(void);

#endif  // _CLIENT_TCP_H_