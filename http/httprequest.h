/*
 * Server 接收到 client 端請求，要進行解析
 */

#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <regex>
#include <unordered_map>
#include <unordered_set>

#include "buffer.h"

enum RequestState 
{
    REQUEST_LINE,
    HEADER,
    BODY,
    FINISH
};
class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    void Init();
    bool Parse(Buffer &buff);

    std::string GetPath();
    bool IsKeepAlive();

private:
    bool ParseRequestLine(const std::string line);
    void ParsePath();
    void ParseHeader(const std::string line);
    void ParseBody(const std::string line);
    void ParsePost();
    void ParseFromURLendocde();

    std::string method_, path_, body_;
    std::string version_;
    RequestState state_;
    std::unordered_map<std::string, std::string> header_;
    std::unordered_map<std::string, std::string> post_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
};

#endif