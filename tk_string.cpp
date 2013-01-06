#include <cstring>
#include <cstdio>
#include "tk_string.h"

using std::vector;

String::String()
{
    _capacity = 16;
    _data = new char[_capacity];
    memset(_data, 0, _capacity);
}

String::String(const String & str)
{
    _capacity = str.size() + 1;
    _data = new char[_capacity];
    memset(_data, 0, _capacity);
    strcpy(_data, str.data());
}

String::String(const char * str, size_t size)
{
    if(str)
    {
        if(size && size < strlen(str))
            _capacity = size + 1;
        else
            _capacity = strlen(str) + 1;
        _data = new char[_capacity];
        memset(_data, 0, _capacity);
        strncpy(_data, str, _capacity - 1);
    }
    else
    {
        _capacity = 16;
        _data = new char[_capacity];
        memset(_data, 0, _capacity);
    }
}

String::String(size_t capacity)
{
    _capacity = capacity;
    _data = new char[_capacity];
    memset(_data, 0, _capacity);
}

String::~String()
{
    delete [] _data;
}

String String::number(int value)
{
    char buf[16] = {0};
    sprintf(buf, "%d", value);
    return buf;
}

String String::number(unsigned int value)
{
    char buf[16] = {0};
    sprintf(buf, "%d", value);
    return buf;
}

String String::number(float value)
{
    char buf[16] = {0};
    sprintf(buf, "%f", value);
    return buf;
}

int String::find(const String & str)
{
    char * p = strstr(_data, str.data());
    if(p == 0)
        return -1;
    else
        return p - _data;
}

size_t String::contain(const String & str)
{
    size_t result = 0;
    char * p = strstr(_data, str.data());
    while(p)
    {
        result++;
        p += str.size();
        p = strstr(p, str.data());
    }
    return result;
}

vector<String> String::split(char delim)
{
    vector<String> result;
    char * sub = strtok(_data, &delim);
    while(sub != 0)
    {
        result.push_back(String(sub));
        sub = strtok(0, &delim);
    }
    return result;
}

String & String::replace(const String & old_part, const String & new_part)
{
    if(find(old_part) == -1)
        return *this;

    // 替换后字符串的长度
    _capacity = strlen(_data) + contain(old_part) * (new_part.size() - old_part.size()) + 1;
    char * str = new char[_capacity];
    memset(str, 0, _capacity);

    char * head = _data;
    char * p = strstr(head, old_part.data());
    while(p)
    {
        strncat(str, head, p - head);
        strcat(str, new_part.data());
        head = p + old_part.size();
        p = strstr(head, old_part.data());
    }
    strcat(str, head);

    delete [] _data;
    _data = str;
    return *this;
}

String & String::trimmed()
{
    char black[4] = {' ', '\t', '\r', '\n'};
    for(int i = 0; i < sizeof(black); i++)
    {
        // 删除头部空白字符
        while(_data[0] == black[i])
        {
            size_t last = strlen(_data) - 1;
            for(size_t j = 0; j < last; j++)
                _data[j] = _data[j + 1];
            _data[last] = '\0';
        }
        // 删除尾部空白字符
        while(_data[strlen(_data) - 1] == black[i])
            _data[strlen(_data) - 1] = '\0';
    }
    return *this;
}

String String::substr(size_t index, size_t size)
{
    if(index >= strlen(_data))
        return String();
    return String(_data + index, size);
}

bool String::startsWith(const String & str)
{
    return (strncmp(_data, str.data(), str.size()) == 0);
}

bool String::endsWith(const String & str)
{
    if(str.size() > strlen(_data))
        return false;
    char * p = _data + strlen(_data) - str.size();
    return (strncmp(p, str.data(), str.size()) == 0);
}

String String::toUpper()
{
    int len = strlen(_data) + 1;
    char * data = new char[len];
    memset(data, 0, len);
    strcpy(data, _data);
    for(int i = 0; i < len; i++)
    {
        if(data[i] >= 'a' && data[i] <= 'z')
            data[i] -= 32;
    }
    String result(data);
    delete [] data;
    return result;
}

String String::toLower()
{
    int len = strlen(_data) + 1;
    char * data = new char[len];
    memset(data, 0, len);
    strcpy(data, _data);
    for(int i = 0; i < len; i++)
    {
        if(data[i] >= 'A' && data[i] <= 'Z')
            data[i] += 32;
    }
    String result(data);
    delete [] data;
    return result;
}

String & String::encode()
{
    replace("&", "&amp;");
    replace("<", "&lt;");
    replace(">", "&gt;");
    replace("\"", "&#34;");
    replace("'", "&#39;");
    replace("\n", "<br/>");
    return *this;
}

String & String::decode()
{
    replace("<br/>", "\n");
    replace("&#39;", "'");
    replace("&#34;", "\"");
    replace("&lt;", "<");
    replace("&gt;", ">");
    replace("&amp;", "&");
    return *this;
}

String & String::filterForSQL()
{
    replace("\\", "\\\\");
    replace("\"", "\\\"");
    replace("'", "\\'");
    return *this;
}

String String::operator+(const char * str) const
{
    size_t len = size() + strlen(str) + 1;
    char * tmp = new char[len];
    memset(tmp, 0, len);
    strcpy(tmp, _data);
    strcat(tmp, str);
    String result(tmp);
    delete [] tmp;
    return result;
}

String String::operator+(const String & str) const
{
    return operator+(str.data());
}

void String::operator+=(const char * str)
{
    size_t len = _capacity;
    while(len < size() + strlen(str) + 1)
        len *= 2;
    if(len != _capacity)
    {
        char * tmp = _data;
        _capacity = len;
        _data = new char[_capacity];
        memset(_data, 0, _capacity);
        strcpy(_data, tmp);
        delete [] tmp;
    }
    strcat(_data, str);
}

void String::operator+=(const String & str)
{
    operator+=(str.data());
}

void String::operator=(const char * str)
{
    size_t len = _capacity;
    while(len < strlen(str) + 1)
        len *= 2;
    if(len != _capacity)
    {
        delete [] _data;
        _capacity = len;
        _data = new char[_capacity];
    }
    memset(_data, 0, _capacity);
    strcpy(_data, str);
}

void String::operator=(const String & str)
{
    return operator=(str.data());
}

bool String::operator==(const char * str) const
{
    return (strcmp(_data, str) == 0);
}

bool String::operator==(const String & str) const
{
    return operator==(str.data());
}

bool String::operator!=(const char * str) const
{
    return (strcmp(_data, str) != 0);
}

bool String::operator!=(const String & str) const
{
    return operator==(str.data());
}

bool String::operator<(const String & str) const
{
    return (strcmp(_data, str.data()) < 0);
}

bool String::operator>(const String & str) const
{
    return (strcmp(_data, str.data()) > 0);
}

char & String::operator[](size_t i)
{
    return _data[i];
}
