#include "buffer.h"


Buffer::Buffer()
{

}

Buffer::~Buffer()
{

}

size_t Buffer::WriteableBytes() const
{
    return buffer_.size() - writepos_;
}

size_t Buffer::ReadableBytes() const
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

char* Buffer::BeginWrite()
{
    return BeginPtr() + writepos_;
}

void Buffer::HasWriten(size_t len)
{
    writepos_ += len;
}

const char* Buffer::Peek() const
{
    return BeginPtr() + readpos_;
}

void Buffer::Append(const char* str, size_t len)
{
    std::copy(str, str + len, BeginWrite());
    HasWriten(len);
}

void Buffer::Append(const std::string str)
{
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len)
{
    Append(static_cast<const char*>(data), len);
}

// void Buffer::Append(const Buffer& buff)
// {
    
// }

ssize_t Buffer::Writefd(int32_t sockfd)
{
    size_t read_size = ReadableBytes();
    ssize_t len = write(sockfd, Peek(), read_size);
    if(len < 0)
        return len;
    readpos_ += len;
    return len;
}

ssize_t Buffer::Readfd(int32_t sockfd)
{
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = WriteableBytes();
    // const size_t 

    iov[0].iov_base = BeginPtr() + writepos_;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const size_t len = readv(sockfd, iov, 2);
    if(len <= writeable)
        writepos_ += len;
    else {
        writepos_ = buffer_.size();
        Append(buff, len - writeable);
    }
    return len;
}