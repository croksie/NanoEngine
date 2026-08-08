#pragma once
#include "core/window.h"

class RHI{

public:
    virtual ~RHI() = default;
    virtual void Initialize(Window* window) = 0;
    virtual void Shutdown() = 0;

    

};