#pragma once

#include <Graphics/Renderer.h>

namespace AEON::Graphics
{

class Window;

class Viewport : public Inherit< Object, Viewport >
{
public:
    static Shared<Viewport> create();
    Viewport()
    :   m_renderer{ Renderer::instance() }
    {};
public:
    virtual bool AdvanceFrame();
    virtual bool AcquireFrame();
    virtual void Update();
    virtual void Present();
protected:
    virtual                 ~Viewport() = default;
protected:
    friend class Window;
    Shared<Renderer>    m_renderer;
    
};
using Viewports = List<Shared<Viewport>>;
    
} // namespace AEON::Graphics