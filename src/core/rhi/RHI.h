#pragma once
#include "core/Window.h"

class RHI{

public:
    virtual ~RHI() = default;
    virtual void Initialize(Window* window) = 0;
    virtual void Shutdown() = 0;

    

};