#include "log.h"
#include "base.h"
#include <stdarg.h>

// static FILE *g_log_fp = NULL;
static const char *g_log_file_path = NULL;

void set_log_path(const char *path) {
    FILE *log_fp = NULL;
    g_log_file_path = path;
    log_fp = fopen(g_log_file_path, "w");
    fclose(log_fp);
    log_fp = NULL;
}

void write_log(const char * format, ...) {
    FILE *log_fp = NULL;
    log_fp = fopen(g_log_file_path, "a");
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vfprintf(log_fp, format, arg_ptr);
    va_end(arg_ptr);
    fclose(log_fp);
    log_fp = NULL;
}