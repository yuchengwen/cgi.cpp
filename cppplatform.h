/*
 * Copyright (c) 2012 Yu Cheng wen
 * yu0736@live.com
 */

#ifndef _CPP_PLATFORM_H_
#define _CPP_PLATFORM_H_

// ÅÐ¶Ï²Ù×÷ÏµÍ³
#if (defined(_WIN32) || defined(_WIN64))
#define CPP_WIN
#endif

#ifdef CPP_WIN
#ifdef CGI_DLL
#define CPP_API __declspec(dllexport)
#else
#define CPP_API __declspec(dllimport)
#endif
#else
#define CPP_API
#endif

#endif
