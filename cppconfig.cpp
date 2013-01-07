#include <cstdio>
#include <cstring>
#include <vector>
#include "cppconfig.h"
#include "cpplog.h"

using namespace std;

Config::Config()
{
}

Config::Config(const String & file)
{
    open(file);
}

void Config::open(const String & file)
{
    _file = file;
    FILE * fp = fopen(file.data(), "r");

    // 判断Session时会试图打开不存在的Session文件, 所以此处不写错误日志
    if(fp == 0)
        return;

    char buf[128] = {0};
    while(fgets(buf, sizeof(buf), fp))
    {
        String line(buf);
        vector<String> pair = line.split('=');
        if(pair.size() != 2)
            continue;
        _config[pair[0]] = pair[1];
    }
    fclose(fp);
}

String Config::get(const String & key)
{
    return _config[key];
}

void Config::set(const String & key, const String & value)
{
    if(key.empty())
        return;

    _config[key] = value;
    FILE * fp = fopen(_file.data(), "w");
    if(fp == 0)
    {
        CPP_ERROR("Can't write config '%s=%s' to %s", key.data(), value.data(), _file.data());
        return;
    }
    for(map<String, String>::iterator it = _config.begin(); it != _config.end(); it++)
    {
        if((*it).first.empty() || (*it).second.empty())
            continue;
        fputs(((*it).first + "=" + (*it).second + "\n").data(), fp);
    }
    fclose(fp);
}
