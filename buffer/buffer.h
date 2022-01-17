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
    Buffer();
    ~Buffer();

    ssize_t Readfd(int32_t sockfd, int* event_errno);
    ssize_t Writefd(int32_t sockfd, int* event_errno);

    size_t WriteBytes() const;
    size_t ReadBytes() const;

    void Append(const std::string stf);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buf);

private:
    std::vector<char> buffer_;

    char* BeginPtr();
    const char* BeginPtr() const;
    void MakeSpace(size_t len);

    std::atomic<std::size_t> readpos_;
    std::atomic<std::size_t> writepos_;
};

#endif