#include "httpresponse.h"

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

const std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

HttpResponse::HttpResponse()
{
    code_ = -1;
    path_ = srcDir_ = "";
    isKeepAlive_ = false;
    file_ = nullptr; 
    file_stat_ = { 0 };
}

HttpResponse::~HttpResponse()
{
    UnmapFile();
}

void HttpResponse::Init(const std::string srcDir, std::string path, bool isKeepAlive, int code)
{
    file_ = nullptr;
    srcDir_ = srcDir;
    path_ = path;
    isKeepAlive_ = isKeepAlive;
    code_ = code;
    file_stat_ = {0};
}

void HttpResponse::MakeResponse(Buffer& buff)
{
    if(stat((srcDir_ + path_).data(), &file_stat_) < 0)
        code_ = 404;
    else if(!(file_stat_.st_mode & S_IROTH))
        code_ = 403;
    else if(code_ == -1)
        code_ = 200;

    ErrorHtml();
    AddStateLine(buff);
    AddHeader(buff);
    AddContent(buff);
}

char* HttpResponse::File()
{
    return file_;
}

size_t HttpResponse::FileLength() const
{
    return file_stat_.st_size;
}

void HttpResponse::ErrorHtml()
{
    if(CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;
        stat((srcDir_ + path_).data(), &file_stat_);
    }
}

void HttpResponse::AddStateLine(Buffer& buff)
{
    std::string status;
    if(CODE_STATUS.count(code_) == 1)
        status = CODE_STATUS.find(code_)->second;
    else {
        code_ = 400;
        status = CODE_STATUS.find(code_)->second;
    }
    buff.Append("HTTP/1.1 " + std::to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::AddHeader(Buffer& buff)
{
    buff.Append("Connection: ");
    if(isKeepAlive_) {
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n");
    }
    else
        buff.Append("close\r\n");
    buff.Append("Content-type: " + GetFileType() + "\r\n");
}

void HttpResponse::AddContent(Buffer& buff)
{
    int srcfd = open((srcDir_ + path_).data(), O_RDONLY);
    if(srcfd < 0) {
        ErrorContent(buff, "File NotFound!");
        return;
    }
    
    int* ret = (int*)mmap(0, file_stat_.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
    if(*ret == -1) {
        ErrorContent(buff, "File NotFound!");
        return;
    }
    file_ = (char*)ret;
    close(srcfd);
    buff.Append("Content-length: " + std::to_string(file_stat_.st_size) + "\r\n\r\n");
}

void HttpResponse::UnmapFile()
{
    if(file_) {
        munmap(file_, file_stat_.st_size);
        file_ = nullptr;
    }
}


std::string HttpResponse::GetFileType() {
    /* 判断文件类型 */
    std::string::size_type idx = path_.find_last_of('.');
    if(idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = path_.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void HttpResponse::ErrorContent(Buffer& buff, std::string message)
{
    std::string body;
    std::string status;

    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";

    if(CODE_STATUS.count(code_) == 1)
        status = CODE_STATUS.find(code_)->second;
    else
        status = "BAD REQUEST";
    
    body += std::to_string(code_) + " : " + status  + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    buff.Append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buff.Append(body);
}