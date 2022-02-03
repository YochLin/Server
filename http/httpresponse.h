/*
 * Server 接收到 client 端請求後要傳送的部分 
 */

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <fcntl.h>      // open
#include <unordered_map>
#include <sys/stat.h>
#include <sys/mman.h>   // mmap

#include "buffer.h"
class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void Init(const std::string srcDir, std::string path, bool isKeepAlive, int code);

    void MakeResponse(Buffer& buff);

    char* File();
    size_t FileLength() const;
    void UnmapFile();
    std::string GetFileType();

    void ErrorContent(Buffer& buff, std::string message);

private:
    void AddStateLine(Buffer& buff);
    void AddHeader(Buffer& buff);
    void AddContent(Buffer& buff); 

    void ErrorHtml();

    int code_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;

    char* file_;
    struct stat file_stat_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif