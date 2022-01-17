#ifndef _HTTP_REQUEST_
#define _HTTP_REQUEST_

#include <string>

class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    void Init();

private:
    std::string method_, path_;
};

#endif