#pragma once


class Buffer {
public:
    virtual ~Buffer() = default;
    virtual void setData(size_t size, const void* data, size_t offset = 0) = 0;
    virtual size_t getSize() = 0;
};