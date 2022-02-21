#include "buffer.h"


Buffer::Buffer(int buffer_size): buffer_(buffer_size), 
                                 writepos_(0), 
                                 readpos_(0)
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

const char* Buffer::BeginWriteConst() const
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

ssize_t Buffer::Writefd(int32_t sockfd, int* saveError)
{
    size_t read_size = ReadableBytes();
    ssize_t len = write(sockfd, Peek(), read_size);
    if(len < 0) {
        *saveError = errno;
        return len;
    }
    readpos_ += len;
    return len;
}

ssize_t Buffer::Readfd(int32_t sockfd, int* saveError)
{
    char buff[65535];
    struct iovec iov[2];
    const size_t writeable = WriteableBytes();
    // const size_t 

    iov[0].iov_base = BeginPtr() + writepos_;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(sockfd, iov, 2);
    // const size_t len = read(sockfd, BeginWrite(), 2);
    if(len < 0) {
        *saveError = errno;
    }
        
    else if(static_cast<size_t>(len) <= writeable) {
        writepos_ += len;
    }
    else {
        writepos_ = buffer_.size();
        Append(buff, len - writeable);
    }
    return len;
}

void Buffer::Retrieve(size_t len)
{
    readpos_ += len;
}

void Buffer::RetrieveUntil(const char* end)
{
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll()
{
    std::fill(buffer_.begin(), buffer_.end(), 0);
    readpos_ = 0;
    writepos_ = 0;
}