#include "client_tcp.h"
#include "log.h"

#define LOG_TAG  "[main]"

int32_t main(void) {
    int32_t ret = 0;
    set_log_path("client/log/client_log.txt");

    do {
        ret = client_tcp_init();
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] client_tcp_init failed!", __func__);
            break;
        }
        LOGI(LOG_TAG "[%s] client_tcp_init success!", __func__);

        while (1) {
            client_tcp_send_from_stdin();
        };

        client_tcp_close();
    } while (0);

    return ret;
}