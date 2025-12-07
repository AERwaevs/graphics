#pragma once

#include <Base/Base.h>
#include <Base/EventListener.h>

#include "VulkanRenderer.h"
#include "Viewport.h"

namespace aer
{
    
struct WindowProperties
{
    std::string name{        "AER"  };
    bool        fullscreen{  false  };
    bool        minimized{   false  };
    bool        borderless{  false  };
    bool        vsync{       false  };
    uint32_t    width{       1280   };
    uint32_t    height{      720    };
    uint32_t    posx{        0      };
    uint32_t    posy{        0      };
    API         api{   API::Vulkan };
    int         screenNum{  -1     };
    std::string display;
    std::string windowClass;
    std::any    nativeWindow;
    std::any    systemConnection;
};


class Window : public Object, public IEventListener<Window>
{
public:
    
    const   WindowProperties&   properties() const { return _properties; }

            void                Update(){};
            Window( const WindowProperties& props )
            : _properties( props ), _renderer( create<VulkanRenderer>() ), _viewport( create<Viewport>( this )) {};
    virtual ~Window() = default;
protected:
            ref_ptr<Renderer>   _renderer;
            ref_ptr<Viewport>   _viewport;
            WindowProperties    _properties;

};
using Windows = std::list<ref_ptr<Window>>;

extern ref_ptr<Window> createWindow( const WindowProperties& = WindowProperties() );

}