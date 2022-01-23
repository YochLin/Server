/*
 * Server 接收到 client 端請求，要進行解析
 */

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>

class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    void Init();

private:
    bool ParseRequestLine(const std::string line);
    void ParseHeader(const std::string line);
    void ParseBody(const std::string line);

    std::string method_, path_;
};

#endif