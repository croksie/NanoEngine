#pragma once

namespace midgard::bifrost {

enum class ShaderType {
    VERTEX,
    FRAGMENT
};

class Shader {
public:
    virtual ~Shader() = default;
};

} // namespace midgard::bifrost