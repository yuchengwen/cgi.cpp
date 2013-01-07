/*
* Copyright (c) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _CPP_LOG_H_
#define _CPP_LOG_H_

#include <cstdio>
#include <cstring>
#include <ctime>
#include "cppplatform.h"

#define _CPP_ERROR_PREFIX\
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

#define _CPP_ERROR_POSTFIX\
        fprintf(log_file, "\n");\
        fclose(log_file);\
    }while(0)

// 将错误信息写入日志文件. 影响性能, 非必要时不调用
#ifdef CPP_WIN
#define CPP_ERROR(fmt, ...)\
    _CPP_ERROR_PREFIX\
    fprintf(log_file, fmt, __VA_ARGS__);\
    _CPP_ERROR_POSTFIX
#else
#define CPP_ERROR(fmt, args...)\
    _CPP_ERROR_PREFIX\
    fprintf(log_file, fmt, ##args);\
    _CPP_ERROR_POSTFIX
#endif

#endif // _CPP_LOG_H_
