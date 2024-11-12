#pragma once

#include <Base/Base.h>

#include "Renderer.h"

namespace aer
{

class Window;

class Viewport : public Object
{
//public:
//    template< API api = API::Default >
//    static ref_ptr<Viewport> create( Window* window );
protected:
    Viewport( Window* window ) : _window( window ), _renderer( Renderer::get_or_create() ) {};
    virtual ~Viewport() noexcept = default;
public:
    virtual bool AdvanceFrame();
    virtual bool AcquireFrame();
    virtual void Update();
    virtual void Present();
protected:
    friend class Window;
    spy_ptr<Window>   _window;
    ref_ptr<Renderer> _renderer;
};
using Viewports = std::list<ref_ptr<Viewport>>;

} // namespace aer