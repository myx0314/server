#include "server_tcp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "log.h"
#include <string.h>
#include <unistd.h>

#define LOG_TAG  "[server_tcp]"

sevent my_events[MAX_EVENT_SIZE + 1];
static int32_t g_epfd;
static void (*g_handle_buf)(char *buf, int32_t *buf_len) = NULL;

static void read_data(int32_t fd, int32_t event, void *arg);

int32_t server_tcp_init(int32_t *lfd) {
    int32_t ret = 0;
    struct sockaddr_in addr = { 0 };
    int32_t opt = 1;
    FUNC_ENTER;

    do {
        if (lfd == NULL) {
            ret = -1;
            LOGE(LOG_TAG "[%s] NULL Pointer!", __func__);
            break;
        }

        // create socket
        *lfd = socket(AF_INET, SOCK_STREAM, 0);
        if (*lfd < 0) {
            LOGE(LOG_TAG "[%s] Creating the listening socket failed!", __func__);
            ret = -1;
            break;
        }

        // make the port able to be reused
        ret = setsockopt(*lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] setsockopt failed!", __func__);
            break;
        }

        // bind
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT_NUM);
        inet_pton(AF_INET, SERVER_IP, &addr.sin_addr.s_addr);
        ret = bind(*lfd, (struct sockaddr *) &addr, sizeof(addr));
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] bind failed!", __func__);
            break;
        }

        // listen
        ret = listen(*lfd, MAX_SOCKET_QUEUE_LENGTH);
        if (ret < 0) {
            LOGE(LOG_TAG "[%s] listen failed!", __func__);
            break;
        }
    } while (0);

    FUNC_EXIT(ret);
    return ret;
}

static void add_event(int32_t fd, int32_t events, void (*call_back)(int32_t, int32_t, void *),
                        void *arg, sevent *ev) {
    struct epoll_event epv = { 0 };
    FUNC_ENTER;

    ev->fd = fd;
    ev->events = events;
    ev->arg = arg;
    ev->call_back = call_back;

    epv.events = events;
    epv.data.ptr = ev;
    epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &epv);

    FUNC_EXIT_VOID;
}

static void set_event(int32_t fd, int32_t events, void (*callback)(int32_t, int32_t, void *),
                        void *arg, sevent *ev) {
    struct epoll_event epv = { 0 };
    FUNC_ENTER;

    ev->fd = fd;
    ev->events = events;
    ev->call_back = callback;

    epv.events = events;
    epv.data.ptr = ev;
    epoll_ctl(g_epfd, EPOLL_CTL_MOD, fd, &epv);
    FUNC_EXIT_VOID;
}

static void del_event(sevent *ev, int32_t fd, int32_t events) {
    struct epoll_event epv = { 0 };
    FUNC_ENTER;

    ev->fd = 0;
    ev->events = 0;
    ev->call_back = NULL;
    memset(ev->buf, 0, sizeof(ev->buf));
    ev->buflen = 0;

    epv.data.ptr = NULL;
    epv.events = events;
    epoll_ctl(g_epfd, EPOLL_CTL_DEL, fd, &epv);
}

static void send_data(int32_t fd, int32_t event, void* arg) {
    sevent *ev = arg;
    FUNC_ENTER;

    write(fd, ev->buf, ev->buflen);
    set_event(fd, EPOLLIN, read_data, arg, ev);         // to add customized func

    FUNC_EXIT_VOID;
}

static void read_data(int32_t fd, int32_t event, void *arg) {
    sevent *ev = arg;
    FUNC_ENTER;

    ev->buflen = read(fd, ev->buf, sizeof(ev->buf));
    if (ev->buflen > 0) {
        if (g_handle_buf != NULL) {
            g_handle_buf(ev->buf, &ev->buflen);
        }
        set_event(fd, EPOLLOUT, send_data, arg, ev);    // to add customized func
    } else if (ev->buflen == 0) {
        close(fd);
        del_event(ev, fd, EPOLLIN);
    }

    FUNC_EXIT_VOID;
}

static void callback_accept(int32_t lfd, int32_t events, void *arg) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int32_t cfd = 0;
    uint32_t i = 0;
    FUNC_ENTER;

    cfd = accept(lfd, (struct sockaddr*)&addr, &len);
    if (cfd < 0) {
        LOGE(LOG_TAG "[%s] accept failed!", __func__);
        return;
    }

    for (i = 0; i < MAX_EVENT_SIZE; i++) {
        if (my_events[i].fd == 0) {
            add_event(cfd, EPOLLIN, read_data, &my_events[i], &my_events[i]);
            break;
        }
    }

    FUNC_EXIT_VOID;
}

int32_t server_tcp_epoll(int32_t lfd, void (*handle_buf)(char*, int32_t*)) {
    int32_t ret = 0;
    FUNC_ENTER;

    do {
        g_epfd = epoll_create(MAX_EVENT_SIZE);
        if (g_epfd < 0) {
            LOGE(LOG_TAG "[%s] epoll_create failed!", __func__);
            ret = -1;
            break;
        }

        g_handle_buf = handle_buf;
        add_event(lfd, EPOLLIN, callback_accept, &my_events[MAX_EVENT_SIZE], &my_events[MAX_EVENT_SIZE]);

        while (1) {
            struct epoll_event events[MAX_EVENT_SIZE];
            int32_t nready = epoll_wait(g_epfd, events, MAX_EVENT_SIZE, -1);
            if (nready < 0) {
                LOGE(LOG_TAG "[%s] epoll_wait failed!", __func__);
            } else if (nready > 0) {
                uint32_t i = 0;
                for (i = 0; i < nready; i++) {
                    sevent *se = events[i].data.ptr;
                    LOGI(LOG_TAG "[%s] fd = %d", __func__, se->fd);

                    if (se->events & events[i].events) {
                        se->call_back(se->fd, se->events, se);
                    }
                }
            }
        }
    } while (0);

    FUNC_EXIT(ret);
    return ret;
}
