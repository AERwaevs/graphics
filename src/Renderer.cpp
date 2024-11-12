#include <Graphics/Window.h>

namespace aer
{

template<> ref_ptr<Renderer> Renderer::get_or_create< API::None >()
{
    AE_FATAL( "Invalid API" );
    return {};
}

} // namespace aer
