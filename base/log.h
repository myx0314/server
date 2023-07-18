#ifndef _LOG_H_
#define _LOG_H_
#include <stdio.h>

#define LOGE(format, ...)   \
    write_log("[E] "format"\r\n", ##__VA_ARGS__)
#define LOGD(format, ...)   \
    write_log("[D] "format"\r\n", ##__VA_ARGS__)
#define LOGI(format, ...)   \
    printf("[I] "format"\r\n", ##__VA_ARGS__);   \
    write_log("[I] "format"\r\n", ##__VA_ARGS__)

#define FUNC_ENTER  LOGD(LOG_TAG "[%s] enter", __func__)
#define FUNC_EXIT(ret)  LOGD(LOG_TAG "[%s] exit, return value: %d", __func__, ret)
#define FUNC_EXIT_VOID  LOGD(LOG_TAG "[%s] exit", __func__)

// void open_log(const char *path);
// void close_log(void);
void set_log_path(const char *path);
void write_log(const char * format, ...);

#endif  // _LOG_H_