/*
* Copyright (C) 2012 Yu Cheng wen
* yu0736@live.com
*/

#ifndef _TK_PAGE_H_
#define _TK_PAGE_H_

#include <map>
#include <vector>
#include "tk_platform.h"
#include "tk_string.h"
#include "tk_form.h"

class Config;

using namespace std;

class TK_API Page
{
public:
    Page();
    ~Page();

    inline Form * form()
    {
        return _form;
    }

    // 获取客户端IP地址
    inline String & clientAddr()
    {
        return _remote_addr;
    }

    // 获取浏览器Cookie值
    inline String & cookie(const String & key)
    {
        return _cookie[key];
    }
    // 保存Cookie到浏览器
    void setCookie(const String & key, const String &value, int second);

    // 获取URL参数
    String pathInfo(size_t index);

    // 获取Session值
    String session(const String & key);
    // 保存Session. 参数live为正数时是有效期, 单位秒
    void setSession(const String & key, const String & value, int live = -1);

    // 网站根目录的绝对路径
    inline String & documentRoot()
    {
        return _document_root;
    }

    // 文件上传目录，需设置为'/'开头的网站绝对路径
    static void setUploadDir(const String & dir)
    {
        _upload_dir = dir;
    }

    // 永久保存上传的文件，返回URL
    String saveUploadFile(const String &file);

    // 设置模板文件存放目录，填'.'开头的相对路径
    static inline void setTmplDir(const String & dir)
    {
        Page::_tmpl_dir = dir;
    }

    // 设置临时文件存放目录，填'.'开头的相对路径
    static inline void setTempDir(const String & dir)
    {
        Page::_temp_dir = dir;
    }

    static inline void setContentType(const String & content_type)
    {
        Page::_content_type = content_type;
    }

    // 页面重定向
    void redirect(const String & url);

    // 页面未找到
    void notFound();

    // 设置模板参数
    inline void response(const String & key, const String & value)
    {
        _response[key] = value;
    }

    // 渲染模板, 显示页面
    void render(const String & tmpl);

private:
    String load(const String & tmpl);
    String randomString();

    Form * _form;
    map<String, String> _cookie;
    map<String, String> _response;
    String _remote_addr;
    int _remote_port;
    String _document_root;
    vector<String> _path_info;
    String _cookie_response;
    Config * _session;

    static String _tmpl_dir;
    static String _temp_dir;
    static String _upload_dir;
    static String _content_type;
};

#endif
