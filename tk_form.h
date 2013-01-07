/*
* Copyright (c) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _TK_FORM_H_
#define _TK_FORM_H_

#include <map>
#include <cstdio>
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
    // 临时文件名
    char name[L_tmpnam];
    // 后缀名
    char postfix[16];
    // 文件大小，单位Kb
    size_t size;
    char mime_type[64];
    Status status;
} FileInfo;

class TK_API Form
{
public:
    Form();
    ~Form();

    inline String & method()
    {
        return _method;
    }

    inline String & get(const String & key)
    {
        return _get[key];
    }

    inline String & post(const String & key)
    {
        return _post[key];
    }

    // 最大上传文件大小, 单位KB
    static inline size_t maxUpdateSize()
    {
        return Form::_max_upload_size;
    }
    static inline void setMaxUploadSize(size_t size)
    {
        Form::_max_upload_size = size;
    }

    // 上传文件的信息
    Status uploadStatus(const String & key);
    size_t uploadSize(const String & key);
    String uploadFile(const String &key);
    String uploadMimeType(const String & key);
    String uploadPostfix(const String & key);
    
    void fileSaved(const String &key);

private:
    // 指针指向行尾.返回换行符的长度
    size_t findEnding(char ** ch);
    // 指针指向下一行行首
    void crossEnding(char ** ch);
    // 指针由行首退回到上一个行尾
    void lastEnding(char ** ch);
    
    String _method;
    map<String, String> _get;
    map<String, String> _post;
    map<String, FileInfo> _upload;

    static size_t _max_upload_size;
};

#endif
