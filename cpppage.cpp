#include <ctime>
#include "cppplatform.h"
#include "cpplog.h"
#include "cppconfig.h"
#include "cpppage.h"

#define TAG_L "<%"
#define TAG_R "%>"
// Session在浏览器上保存的识别码
#define SESSION_COOKIE_NAME "sid"
#define SESSION_FILE_TAG Page::_temp_dir + "sess_"

String Page::_tmpl_dir = "./";
String Page::_temp_dir = "./";
String Page::_upload_dir = "/";
// 默认编码为UTF-8
String Page::_content_type = "Content-type: text/html; charset=utf-8";

Page::Page()
{
    _form = new Form;
    _remote_addr = String(getenv("REMOTE_ADDR"));
    _remote_port = atoi(getenv("REMOTE_PORT"));
    _document_root = String(getenv("DOCUMENT_ROOT"));
    _session = 0;

    // 获取Cookie
    vector<String> cookie = String(getenv("HTTP_COOKIE")).split(';');
    for(size_t i = 0; i < cookie.size(); i++)
    {
        vector<String> pair = cookie[i].split('=');
        if(pair.size() != 2)
            continue;
        _cookie[pair[0]] = pair[1];
    }

    _path_info = String(getenv("PATH_INFO")).split('/');
    if(!_path_info.empty() && _path_info[0].empty())
        _path_info.erase(_path_info.begin());
}

Page::~Page()
{
    if(_session)
        delete _session;
    if(_form)
        delete _form;
}

void Page::setCookie(const String & key, const String &value, int second)
{
    char buf[128] = {0};
    if(second >= 0)
        sprintf(buf, "Set-Cookie: %s=%s; Max-Age=%d\r\n", key.data(), value.data(), second);
    else
        sprintf(buf, "Set-Cookie: %s=%s\r\n", key.data(), value.data());
    _cookie_response += buf;
}

String Page::saveUploadFile(const String &key)
{
    if(_form->uploadStatus(key) != Success)
        return "";

    String filename = Page::_upload_dir + randomString();
    String postfix = _form->uploadPostfix(key);
    if(!postfix.empty())
        filename += "." + postfix;
    if(rename(_form->uploadFile(key).data(), (_document_root + filename).data()) == 0)
    {
        _form->fileSaved(key);
        return filename;
    }
    else
    {
        CPP_ERROR("Move file failed");
        return "";
    }
}

String Page::pathInfo(size_t index)
{
    if(index + 1 > _path_info.size())
        return "";
    return _path_info[index];
}

String Page::session(const String & key)
{
    String sid = _cookie[SESSION_COOKIE_NAME];
    if(sid.empty())
        return "";

    if(!_session)
        _session = new Config(SESSION_FILE_TAG + sid);

    return _session->get(key);
}

void Page::setSession(const String & key, const String & value, int live)
{
    String sid = _cookie[SESSION_COOKIE_NAME];
    if(sid.empty())
    {
        sid = randomString();
        // 保存标识到浏览器
        setCookie(SESSION_COOKIE_NAME, sid, live);
    }

    // 保存会话值到服务器
    if(!_session)
        _session = new Config(SESSION_FILE_TAG + sid);
    _session->set(key, value);
}

void Page::redirect(const String & url)
{
    printf("%s\r\n", Page::_content_type.data());
    printf("Location: %s\r\n", url.data());
    if(!_cookie_response.empty())
        printf("%s", _cookie_response.data());
    printf("\r\n");
}

void Page::notFound()
{
    CPP_ERROR("Page not found: %s", getenv("REQUEST_URI"));
    printf("%s\r\n\r\nPage not found", Page::_content_type.data());
}

String Page::load(const String & tmpl)
{
    String html(1024);
    // 打开模板文件
    FILE * fp = fopen((Page::_tmpl_dir + tmpl).data(), "r");
    if(!fp)
    {
        CPP_ERROR("Can't load tmpl file: %s", tmpl.data());
        return "";
    }

    // 加载模板，渲染
    char buf[1025] = {0};
    while(fgets(buf, sizeof(buf), fp))
    {
        String line(buf);

        // 检查是否有包含其他模板文件
        String tag = String(TAG_L) + "inc ";
        size_t idx_0 = line.find(tag);
        size_t idx_1 = line.find(TAG_R);
        if(idx_0 != -1 && idx_1 != -1)
        {
            size_t start = idx_0 + tag.size();
            html += load(line.substr(start, idx_1 - start));
            continue;
        }

        // 变量替换
        for(map<String, String>::iterator it = _response.begin(); it != _response.end(); it++)
            line.replace(String(TAG_L) + it->first + TAG_R, it->second);
        html += line;
    }

    fclose(fp);
    return html;
}

void Page::render(const String & tmpl)
{
    // 输出HTTP头
    printf("%s\r\n", Page::_content_type.data());
    if(!_cookie_response.empty())
        printf("%s", _cookie_response.data());
    printf("\r\n");

    // 输出HTML
    printf("%s", load(tmpl).data());
}

String Page::randomString()
{
    // 利用客户端IP，端口和当前时间生成一个随机且唯一的字符串
    static unsigned int index = 0;
    vector<String> ip = _remote_addr.split('.');
    char buf[64] = {0};
    sprintf(buf, "%X%X%X%X%X%X%X", ip[0].toLong(), ip[1].toLong(), ip[2].toLong(), ip[3].toLong(), _remote_port, time(0), index++);
    return buf;
}
