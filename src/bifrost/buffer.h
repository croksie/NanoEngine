#pragma once
#include <cstddef>
#include <cstdint>

namespace midgard::bifrost {

enum class BufferType {
    VERTEX = 0,
    INDEX = 1,
    UNIFORM = 2,
    STORAGE = 3,
    TRANSFER = 4
};

struct BufferDesc {
    BufferType type = BufferType::VERTEX;
    size_t size = 0;
    const void* initData = nullptr;
};

class Buffer {
public:
    virtual ~Buffer() = default;
    virtual void setData(size_t size, const void* data, size_t offset = 0) = 0;
    virtual BufferType getType() const = 0;
    virtual size_t getSize() const = 0;
};

} // namespace midgard::bifrost
