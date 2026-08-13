#pragma once


enum class ShaderType {
    VERTEX,
    FRAGMENT
};

class Shader {
public:
    virtual ~Shader() = default;

};