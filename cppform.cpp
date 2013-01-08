#include <cstdio>
#include <cstring>
#include <vector>
#include "cppplatform.h"
#ifdef CPP_WIN
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif
#include "cpplog.h"
#include "cppform.h"

// POST报文块头部每行数据最长长度
#define POST_LINE_LEN_MAX 512

using namespace std;

// 默认最大上传文件大小为10MB
size_t Form::_max_upload_size = 10240;

Form::Form()
{
#ifdef CPP_WIN
    // 必须设置输入输出为二进制模式，否则POST读取数据可能不完整
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    _method = String(getenv("REQUEST_METHOD")).toUpper();
    String param;
    if(_method == "GET")
        param = getenv("QUERY_STRING");
    else
    {
        int post_len = atoi(getenv("CONTENT_LENGTH"));
        char * post_data = new (nothrow) char[post_len];
        if(!post_data)
        {
            CPP_ERROR("Can't malloc for POST data");
            return;
        }

        if(fread(post_data, 1, post_len, stdin) != post_len)
            CPP_ERROR("Get post data error");

        String content_type = getenv("CONTENT_TYPE");
        if(!content_type.startsWith("multipart/form-data"))
            param = String(post_data, post_len);
        else
        {
            // POST数据分隔符
            String boundary;
            vector<String> p = content_type.split(';');
            if(p.size() == 2)
            {
                p = p[1].split('=');
                if(p.size() == 2 && p[0] == "boundary")
                    boundary = "--" + p[1];
            }

            char * start = post_data;
            char * end = &post_data[post_len - 1] + 1;
            // 按分隔符将数据分块
            vector<char*> index;
            while(start < end)
            {
                int ret = memcmp(start, boundary.data(), boundary.size());
                if(ret == 0)
                {
                    char * ch = start + boundary.size();
                    if(*ch == '-' && *(ch + 1) == '-')
                    {
                        end = start;
                        lastEnding(&end);
                        break;
                    }
                    else
                    {
                        index.push_back(start);
                        crossEnding(&start);
                    }
                }
                else
                    start++;
            }

            // 按块逐个解析
            for(size_t i = 0; i < index.size(); i++)
            {
                char * pos = index[i];
                // 越过分隔符
                crossEnding(&pos);

                char * line = pos;
                size_t ending_len = findEnding(&pos);
                p = String(line, pos - line).split(';');
                map<String, String> disposition;
                for(size_t j = 1; j < p.size(); j++)
                {
                    vector<String> pair = p[j].split('=');
                    if(pair.size() == 2)
                        disposition[pair[0]] = pair[1].substr(1, pair[1].size() - 2);
                }
                pos += ending_len;

                if(disposition.find("name") != disposition.end() && disposition.find("filename") != disposition.end())
                {
                    // 文件上传
                    FileInfo file_info;
                    memset(&file_info, 0, sizeof(file_info));

                    // 获取文件Mime-Type
                    line = pos;
                    ending_len = findEnding(&pos);
                    vector<String> mime_pair = String(line, pos - line).split(':');
                    if(mime_pair.size() == 2 && mime_pair[0] == "Content-Type")
                        strncpy(file_info.mime_type, mime_pair[1].data(), sizeof(file_info.mime_type));

                    pos += ending_len;
                    // 越过空白行
                    crossEnding(&pos);

                    // 计算文件大小
                    if(i == index.size() - 1)
                        file_info.size = end - pos;
                    else
                    {
                        line = index[i + 1];
                        lastEnding(&line);
                        file_info.size = line - pos;
                    }
                    
                    // 后缀名
                    vector<String> name_list = disposition["filename"].split('.');
                    if(name_list.size() > 1)
                        strncpy(file_info.postfix, name_list[name_list.size() - 1].data(), sizeof(file_info.postfix));

                    if(file_info.size / 1024 > Form::_max_upload_size)
                        file_info.status = Oversize;
                    else
                    {
                        tmpnam(file_info.name);
                        FILE * file = fopen(file_info.name, "wb");
                        if(file)
                        {
                            fwrite(pos, 1, file_info.size, file);
                            fclose(file);
                            file_info.size /= 1024;
                        }
                        else
                        {
                            file_info.status = Failed;
                            CPP_ERROR("upload file failed, path: %s", file_info.name);
                        }
                    }
                    _upload[disposition["name"]] = file_info;
                }
                else
                {
                    if(disposition.find("name") != disposition.end())
                    {
                        // 简单的POST参数
                        crossEnding(&pos);
                        size_t size = 0;
                        if(i == index.size() - 1)
                            size = end - pos;
                        else
                        {
                            line = index[i + 1];
                            lastEnding(&line);
                            size = line - pos;
                        }
                        _post[disposition["name"]] = String(pos, size);
                    }
                }
            } // 遍历解析POST块结束
        } // 未编码的POST解析结束
        delete [] post_data;
    }// POST解析结束

    // 解析参数
    vector<String> params = param.split('&');
    for(size_t i = 0; i < params.size(); i++)
    {
        vector<String> pair = params[i].split('=');
        if(pair.size() != 2)
            continue;

        // 浏览器发送过来的数据是编码过了的，需要解码
        int pos = 0;
        size_t len = pair[1].size();
        char * dest = new char[len + 1];
        for(size_t i = 0; i < len; i++)
        {
            char c = pair[1][i];
            if(c == '+')
                dest[pos++] = ' ';
            else if(c == '%')
            {
                int escaped = 0;
                char buf[2] = {0};
                buf[0] = pair[1][++i];
                escaped = strtol(buf, 0, 16) << 4;
                buf[0] = pair[1][++i];
                escaped += strtol(buf, 0, 16);
                dest[pos++] = escaped;
            }
            else
                dest[pos++] = c;
        }
        dest[pos] = '\0';
        pair[1] = dest;
        delete [] dest;

        if(_method == "GET")
            _get[pair[0]] = pair[1];
        else
            _post[pair[0]] = pair[1];
    }
}

Form::~Form()
{
    // 删除上传的文件
    for(map<String, FileInfo>::iterator it = _upload.begin(); it != _upload.end(); it++)
    {
        if(it->second.status == Success && access(it->second.name, 0) == 0)
        {
            if(remove(it->second.name) != 0)
                CPP_ERROR("Delete file failed: %s", it->second.name);
        }
    }
}

Status Form::uploadStatus(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return Failed;
    return _upload[key].status;
}

size_t Form::uploadSize(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return 0;
    return _upload[key].size;
}

String Form::uploadFile(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return "";
    return _upload[key].name;
}

String Form::uploadMimeType(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return "";
    return _upload[key].mime_type;
}

String Form::uploadPostfix(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return "";
    return _upload[key].postfix;
}

void Form::fileSaved(const String & key)
{
    if(_upload.find(key) == _upload.end())
        return;
    _upload[key].status = Saved;
}

size_t Form::findEnding(char ** ch)
{
    // 原本便是指向回车/换行符
    char * str = *ch;
    if(*str =='\n')
        return 1;
    if(*str == '\r')
    {
        if(*(str + 1) == '\n')
            return 2;
        else
            return 1;
    }

    char * pos = (char*)memchr(str, '\r', POST_LINE_LEN_MAX);
    if(pos)
    {
        *ch = pos;
        if(++pos && *pos == '\n')
            return 2;
        else
            return 1;
    }
    else
    {
        pos = (char*)memchr(str, '\n', POST_LINE_LEN_MAX);
        if(pos)
        {
            *ch = pos;
            return 1;
        }
    }
    return 0;
}

void Form::crossEnding(char ** ch)
{
    size_t len = findEnding(ch);
    if(len)
    {
        char * str = *ch;
        str += len;
        *ch = str;
    }
}

void Form::lastEnding(char ** ch)
{
    char * str = *ch;
    char * ch_0 = str - 2;
    char * ch_1 = str - 1;
    if(ch_0 && ch_1 && *ch_0 == '\r' && *ch_1 == '\n')
    {
        str -= 2;
        *ch = str;
        return;
    }
    if(ch_1 && (*ch_1 == '\r' || *ch_1 == '\n'))
    {
        str -= 1;
        *ch = str;
    }
}
