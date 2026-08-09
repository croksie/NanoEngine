#pragma once
#include "core/rhi/rhi.h"

#include <core/window.h>

class OpenGLRHI : public RHI {

public:
    void Initialize(Window* window) override;
    void Shutdown() override {}


private:
    Window* m_window = nullptr;
};