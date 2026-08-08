#pragma once
#include "core/rhi/RHI.h"

#include <core/Window.h>

class OpenGLRHI : public RHI {

public:
    void Initialize(Window* window) override;
    void Shutdown() override {}


private:
    Window* m_window = nullptr;
};