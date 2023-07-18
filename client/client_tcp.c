#include "client_tcp.h"
#include "log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LOG_TAG  "[client_tcp]"

static int32_t g_sock_fd = 0;

int32_t client_tcp_init(void) {
    int32_t ret = 0;
    struct sockaddr_in addr = { 0 };
    FUNC_ENTER;

    do {
        // create the client socket
        g_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (g_sock_fd < 0) {
            LOGE(LOG_TAG "[%s] Creating the client socket failed!", __func__);
            ret = -1;
            break;
        }

        // connect with the server
        addr.sin_family = AF_INET;
        addr.sin_port = htons(SERVER_PORT_NUM);
        inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
        ret = connect(g_sock_fd, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] connect failed!", __func__);
            break;
        }
    } while (0);

    FUNC_EXIT(ret);
    return ret;
}

int32_t client_tcp_send_from_stdin(void) {
    int32_t ret = 0;
    int32_t readn = 0;
    char buf[1024] = { 0 };
    FUNC_ENTER;

    do {
        readn = read(STDIN_FILENO, buf, sizeof(buf));
        if (readn <= 0) {
            ret = -1;
            LOGE(LOG_TAG "[%s] Read from stdin failed!", __func__);
            break;
        }

        if (g_sock_fd <= 0) {
            ret = -1;
            LOGE(LOG_TAG "[%s] Socket has not been created!", __func__);
            break;
        }
        LOGI(LOG_TAG "[%s] send message: %s", __func__, buf);
        write(g_sock_fd, buf, readn);
    } while (0);

    FUNC_EXIT(ret);
}

void client_tcp_close(void) {
    FUNC_ENTER;

    do {
        if (g_sock_fd <= 0) {
            LOGE(LOG_TAG "[%s] Socket has not been created!", __func__);
            break;
        }
        close(g_sock_fd);
    } while (0);

    FUNC_EXIT_VOID;
}