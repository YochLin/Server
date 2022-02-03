#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <errno.h>
#include <unistd.h>
#include <sys/uio.h>
#include <vector>
#include <atomic>
#include <string>

class Buffer
{
public:
    Buffer(int buffer_size=1024);
    ~Buffer();

    ssize_t Readfd(int32_t sockfd);
    ssize_t Writefd(int32_t sockfd);

    size_t WriteableBytes() const;
    size_t ReadableBytes() const;

    char* BeginWrite();
    const char* BeginWriteConst() const;
    void HasWriten(size_t len);

    const char* Peek() const;

    void Append(const std::string str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    // void Append(const Buffer& buf);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);
    void RetrieveAll();

private:
    std::vector<char> buffer_;

    char* BeginPtr();
    const char* BeginPtr() const;
    void MakeSpace(size_t len);

    std::atomic<std::size_t> readpos_;
    std::atomic<std::size_t> writepos_;
};

#endif