/*
 * Copyright (c) 2012 Yu Cheng wen
 * yu0736@live.com
 */

#ifndef _TK_PLATFORM_H_
#define _TK_PLATFORM_H_

// ÅÐ¶Ï²Ù×÷ÏµÍ³
#if (defined(_WIN32) || defined(_WIN64))
#define TK_WIN
#endif

#ifdef TK_WIN
#ifdef TK_DLL
#define TK_API __declspec(dllexport)
#else
#define TK_API __declspec(dllimport)
#endif
#else
#define TK_API
#endif

#endif
