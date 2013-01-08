/*
* Copyright (c) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _CPP_STRING_H_
#define _CPP_STRING_H_

#include <vector>
#include <cstring>
#include <cstdlib>
#include "cppplatform.h"

class CPP_API String
{
public:
    String();
    String(const String & str);
    String(const char * str, size_t size = 0);
    String(size_t capacity);
    ~String();

    inline char * data() const
    {
        return _data;
    }
    inline size_t size() const
    {
        return strlen(_data);
    }
    inline size_t capacity() const
    {
        return _capacity;
    }

    inline bool empty() const
    {
        return (strlen(_data) == 0);
    }
    // 查找子字符串,失败返回-1
    int find(const String & str);
    // 子字符串出现次数
    size_t contain(const String & str);
    // 分隔字符串
    std::vector<String> split(char delim, bool trimmed = true);
    // 字符串替换
    String & replace(const String & old_part, const String & new_part);
    // 删除首尾空白字符
    String & trimmed();
    // 子字符串
    String substr(size_t index, size_t size = 0);
    // 判断字符串是否以某串开头
    bool startsWith(const String & str);
    // 判断字符串是否以某串结尾
    bool endsWith(const String & str);
    // 大写
    String toUpper();
    // 小写
    String toLower();
    // 过滤HTML字符
    String & encode();
    // 反过滤HTML字符
    String & decode();
    // 过滤SQL危险字符
    String & filterForSQL();

    inline long toLong(int base = 10) const
    {
        return strtol(_data, 0, base);
    }
    inline double toDouble() const
    {
        return strtod(_data, 0);
    }

    static String number(int value);
    static String number(unsigned int value);
    static String number(float value);

    String operator+(const String & str) const;
    String operator+(const char * str) const;
    friend String CPP_API operator+(const char * str1, const String & str2);
    void operator=(const String & str);
    void operator=(const char * str);
    void operator+=(const String & str);
    void operator+=(const char * str);
    bool operator==(const String & str) const;
    bool operator==(const char * str) const;
    bool operator!=(const String & str) const;
    bool operator!=(const char * str) const;
    bool operator<(const String & str) const;
    bool operator>(const String & str) const;
    char & operator[](size_t i);

private:
    char * _data;
    size_t _capacity;
};

#endif
