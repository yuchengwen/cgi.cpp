/*
 * Copyright (c) 2012 Yu Cheng wen
 * yu0736@live.com
 */

#ifndef _CPP_CONFIG_H_
#define _CPP_CONFIG_H_

#include <map>
#include "cppstr.h"
#include "cppplatform.h"

using std::map;

/**
 * 配置类, 以类似INI格式保存配置信息到文件
 */
class CPP_API Config
{
public:
    Config();
    Config(const String & file);
    void open(const String & file);
    String get(const String & key);
    void set(const String & key, const String & value);

private:
    map<String, String> _config;
    String _file;
};

#endif
