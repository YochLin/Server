#include "buffer.h"


Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

size_t Buffer::WriteBytes() const
{
    return buffer_.size() - writepos_;
}

size_t Buffer::ReadBytes() const
{
    return writepos_ - readpos_;
}

char* Buffer::BeginPtr()
{
    return &*buffer_.begin();
}

const char* Buffer::BeginPtr() const 
{
    return &*buffer_.begin();
}

ssize_t Buffer::Readfd(int32_t sockfd, int* event_errno)
{
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = WriteBytes();
    // const size_t 

    iov[0].iov_base = BeginPtr() + writepos_;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const size_t len = readv(sockfd, iov, 2);
    if(len < 0) {
        *event_errno = errno;
    }
}