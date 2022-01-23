/*
 * Server 接收到 client 端請求後要傳送的部分 
 */

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "buffer.h"
class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void Init();

private:
    void AddStateLine();
    void AddHeader();
    void AddContent();
}

#endif