/*
* Copyright (c) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _TK_LOG_H_
#define _TK_LOG_H_

#include <cstdio>
#include <cstring>
#include <ctime>
#include "tk_platform.h"

#define _CGI_ERROR_PREFIX\
    do\
    {\
        time_t now = time(0);\
        struct tm * timeinfo = localtime(&now);\
        char buf[64] = {0};\
        strftime(buf, sizeof(buf), "./run_%Y%m%d.log", timeinfo);\
        FILE * log_file = fopen(buf, "a");\
        memset(buf, 0, sizeof(buf));\
        strftime(buf, sizeof(buf), "[%H:%M:%S] ", timeinfo);\
        fprintf(log_file, buf);

#define _CGI_ERROR_POSTFIX\
        fprintf(log_file, "\n");\
        fclose(log_file);\
    }while(0)

// 将错误信息写入日志文件. 影响性能, 非必要时不调用
#ifdef TK_WIN
#define CGI_ERROR(fmt, ...)\
    _CGI_ERROR_PREFIX\
    fprintf(log_file, fmt, __VA_ARGS__);\
    _CGI_ERROR_POSTFIX
#else
#define CGI_ERROR(fmt, args...)\
    _CGI_ERROR_PREFIX\
    fprintf(log_file, fmt, ##args);\
    _CGI_ERROR_POSTFIX
#endif

#endif // _TK_LOG_H_
