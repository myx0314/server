#include "server_tcp.h"
#include "log.h"
#include "server_handle_message.h"

#define LOG_TAG  "[main]"

int32_t main(void) {
    int32_t ret = 0;
    int32_t lfd = 0;

    do {
        ret = server_tcp_init(&lfd);
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] server_tcp_init failed!", __func__);
            break;
        }
        LOGI(LOG_TAG "[%s] server_tcp_init success!", __func__);

        ret = server_tcp_epoll(lfd, server_handle_message);
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] server_tcp_epoll failed!", __func__);
            break;
        }
    } while (0);

    return(ret);
}