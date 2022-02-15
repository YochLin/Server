#include "httprequest.h"

const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML {
    "/index", "/register", "login",
    "/welcome", "/video", "/picture"
};

const std::unordered_map<std::string, int> HttpRequest::DEFAULT_HTML_TAG {
    {"/register.html", 0}, {"/login.html", 1}
};

HttpRequest::HttpRequest()
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::Init()
{
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::Parse(Buffer &buff)
{
    if(buff.ReadableBytes() <= 0)
        return false; 

    const char CRLF[] = "\r\n";
    while(buff.ReadableBytes() && state_ != FINISH) {
        const char* lineEnd = std::search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF+2);
        std::string line(buff.Peek(), lineEnd);
        switch(state_) {
            case REQUEST_LINE:
                if(!ParseRequestLine(line))
                    return false;
                ParsePath();
                break;
            case HEADER:
                ParseHeader(line);
                if(buff.ReadableBytes() <= 2)
                    state_ = FINISH;
                break;
            case BODY:
                ParseBody(line);
                break;
            default:
                break;
        }
        if(lineEnd == buff.BeginWrite())
            break;
        buff.RetrieveUntil(lineEnd + 2);
    }

    printf("func:[%s], %d, [%s], [%s], [%s]\n", __func__, __LINE__, method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

void HttpRequest::ParsePath()
{
    if(path_ == "/")
        path_ = "index.html";
    else {
        for(auto item : DEFAULT_HTML) {
            if(item == path_) {
                path_ += ".html";
                break;
            }
        }
    }
}

bool HttpRequest::ParseRequestLine(const std::string line)
{
    std::regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch submatch;
    if(std::regex_match(line, submatch, pattern)) {
        method_ = submatch[1];
        path_ = submatch[2];
        version_ = submatch[3];
        state_ = HEADER;
        return true;
    }
    return false;
}

void HttpRequest::ParseHeader(const std::string line)
{
    std::regex pattern("^([^]*): ?(.*)$");
    std::smatch submatch;
    if(std::regex_match(line, submatch, pattern)) {
        header_[submatch[1]] = submatch[2];
    }
    else
        state_ = BODY;
}

void HttpRequest::ParseBody(const std::string line)
{
    body_ = line;
    ParsePost();
    state_ = FINISH;
}

void HttpRequest::ParsePost()
{
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {
        ParseFromURLendocde();
        if(DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            printf("func:%s, %d, tag: %d\n", __func__, __LINE__, tag);
        }
    }
}

void HttpRequest::ParseFromURLendocde()
{
    if(body_.size() == 0)
        return;

    std::string key, value;
    char ch;
    int i = 0, j = 0;
    for(i = 0; i < body_.size(); i++) {
        ch = body_[i];
        switch(ch) {

            case '&':
                value = body_.substr(j, i - j);
                j = i + 1;
                post_[key] = value;
                printf("func:%s, %d, %s = %s\n", __func__, __LINE__, key.c_str(), value.c_str());
        }
    }

    if(post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

std::string HttpRequest::GetPath()
{
    return path_;
}

bool HttpRequest::IsKeepAlive()
{
    if(header_.count("Connection") == 1)
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    return false;
}