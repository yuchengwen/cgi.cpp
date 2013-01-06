/*
* Copyright (c) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _TK_FORM_H_
#define _TK_FORM_H_

#include <map>
#include "tk_string.h"
#include "tk_platform.h"

using std::map;

enum Status
{
    Success = 0,
    Failed,
    Oversize,
    Saved
};

// 上传文件的信息
typedef struct
{
    // 不带路径的文件名
    char name[64];
    // 文件大小，单位Kb
    size_t size;
    char mime_type[64];
    Status status;
} FileInfo;

class TK_API Form
{
public:
    Form();

    inline String & method()
    {
        return _method;
    }

private:
    // 指针指向行尾.返回换行符的长度
    size_t findEnding(char ** ch);
    // 指针指向下一行行首
    void crossEnding(char ** ch);
    // 指针由行首退回到上一个行尾
    void lastEnding(char ** ch);
    
    map<String, String> _get;
    map<String, String> _post;
    map<String, FileInfo> _upload;

    String _method;
};

#endif
