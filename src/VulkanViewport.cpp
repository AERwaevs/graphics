#include <Graphics/VulkanViewport.h>
#include <Graphics/VulkanRenderer.h>

#include <vk/RenderPass.h>
#include <vk/PipelineLayout.h>
#include <vk/CommandBuffer.h>
#include <vk/GraphicsPipeline.h>
#include <vk/Surface.h>

#include <vk/state/DynamicState.h>
#include <vk/state/VertexInputState.h>
#include <vk/state/InputAssemblyState.h>
#include <vk/state/ViewportState.h>
#include <vk/state/RasterizationState.h>
#include <vk/state/MultisampleState.h>
#include <vk/state/DepthStencilState.h>
#include <vk/state/ColorBlendState.h>

namespace aer
{
template< typename T >
inline auto read( const std::string& path )
{
    std::ifstream file( path, std::ios::ate | std::ios::binary );
    CHECK_F( !file.is_open(), "Failed to open file: %s", path.c_str() );

    std::size_t         size( file.tellg() );
    std::vector<char>   buffer( size );

    file.seekg( 0 );
    file.read( buffer.data(), size );
    file.close();

    return ref_ptr<T>( new T( buffer ) );
}

const inline auto GetQueueSettings( vk::PhysicalDevice* physical_device, vk::Surface* surface )
{
    auto [ present_family, graphics_family ]
    {
        physical_device->GetQueueFamilies( VK_QUEUE_GRAPHICS_BIT, surface )
    };
    return vk::QueueSettings
    {
        vk::QueueSetting{ graphics_family,{ 1.0f } },
        vk::QueueSetting{ present_family, { 1.0f } },
    };
}

VulkanViewport::VulkanViewport( Window* window )
:   Viewport( window ),
    _instance( vk::Instance::get_or_create() ),
    _surface( create<vk::Surface>
    (
        _instance.get(),
#if defined( AER_PLATFORM_WINDOWS )
        std::any_cast<HWND>(window->properties().nativeWindow)
#elif defined( AER_PLATFORM_LINUX )
        std::any_cast<xcb_connection_t*>(window->properties().systemConnection),
        std::any_cast<xcb_window_t>(window->properties().nativeWindow)
#endif
    )),
    _physical_device( _instance->physical_device( VK_QUEUE_GRAPHICS_BIT, _surface.get() ) ),
    _device( create<vk::Device>
    (
        _physical_device, _surface, GetQueueSettings( _physical_device.get(), _surface.get() )
    )),
    _context( create<vk::Context>( _device )),
    _swapchain( create<vk::Swapchain>
    (
        _physical_device, _device, _surface,
        window->properties().width, window->properties().height, _swapchain_prefs
    ))
{
    using namespace vk;

    _context->renderPass = createRenderPass( _device, _swapchain->format() );
    const auto maxSamples = [&]()
    {
        VkSampleCountFlagBits maxSamples( VK_SAMPLE_COUNT_1_BIT );
        for( const auto& attachment : _context->renderPass->attachments )
        {
            maxSamples = std::max( maxSamples, attachment.samples );
        }
        return maxSamples;
    }();

    _context->states =
    {
        create<DynamicState>(),
        create<VertexInputState>(),
        create<InputAssemblyState>(),
        create<ViewportState>( _swapchain->extent() ),
        create<RasterizationState>(),
        create<MultisampleState>( maxSamples ),  //TODO - this needs to match the renderpass color and depth attachment
        create<DepthStencilState>(),
        create<ColorBlendState>()
    };

    //* create shaders
    auto vert_code   = read<ByteCode>( "shaders/tri.vert.spv" );
    auto frag_code   = read<ByteCode>( "shaders/tri.frag.spv" );
    auto vert_module = create<ShaderModule>( _device, *vert_code );
    auto frag_module = create<ShaderModule>( _device, *frag_code );
    ShaderStages stages
    {
        create<ShaderStage>( VK_SHADER_STAGE_VERTEX_BIT, vert_module, "main" ),
        create<ShaderStage>( VK_SHADER_STAGE_FRAGMENT_BIT, frag_module, "main" )
    };
    
    _pipelineLayout = create<PipelineLayout>( _context->device );
    _pipelineLayout->Compile( *_context );
    _graphicsPipeline = create<GraphicsPipeline>( _pipelineLayout, stages, _context->states );
    _graphicsPipeline->Compile( *_context );

    // TODO - handle depth
    //_depthImage = Image::create();
    //_depthImage->imageType          = VK_IMAGE_TYPE_2D;
    //_depthImage->format             = VK_FORMAT_D32_SFLOAT;
    //_depthImage->extent             = { _swapchain->extent().width, _swapchain->extent().height, 1 };
    //_depthImage->mipLevels          = 1;
    //_depthImage->arrayLayers        = 1;
    //_depthImage->samples            = maxSamples;
    //_depthImage->tiling             = VK_IMAGE_TILING_OPTIMAL;
    //_depthImage->usage              = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    //_depthImage->sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
    //_depthImage->queueFamilyIndices = {};
    //_depthImage->layout             = VK_IMAGE_LAYOUT_UNDEFINED;
    //_depthImage->Compile( _device );

    //_depthImageView = ImageView::create( _depthImage );
    //_depthImageView->Compile( _device );
    
    //* create framebuffer
    const auto& imageViews = _swapchain->imageViews();
    for( std::size_t i = 0; i < imageViews.size(); i++ )
    {
        std::vector<ref_ptr<ImageView>> attachments { imageViews[i] };
        _framebuffers.push_back( create<Framebuffer>( _context->renderPass, attachments, _swapchain->extent().width, _swapchain->extent().height, 1 ) );
    }

    //* create command pool
    auto graphicsFamily = [&]
    {
        uint32_t graphicsFamily;
        std::tie( graphicsFamily, std::ignore ) = _physical_device->GetQueueFamilies( VK_QUEUE_GRAPHICS_BIT, _surface.get() );
        return graphicsFamily;
    }();
    _commandPool    = create<CommandPool>( _device, graphicsFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

    //TODO create command buffer
    _commandBuffer  = _commandPool->allocate();
    _commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );

    //TODO begin render pass
    VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
    VkRenderPassBeginInfo renderPassInfo
    {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass      = *_context->renderPass,
        .framebuffer     = *_framebuffers[0],
        .renderArea      = VkRect2D{ VkOffset2D{ 0, 0 }, _swapchain->extent() },
        .clearValueCount = 1,
        .pClearValues    = &clearColor
    };
    VkViewport viewport
    {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>( _swapchain->extent().width ),
        .height   = static_cast<float>( _swapchain->extent().height ),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor
    {
        .offset = { 0, 0 },
        .extent = _swapchain->extent()
    };

    //TODO record commands
    vkCmdBeginRenderPass( *_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
    vkCmdBindPipeline( *_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_graphicsPipeline );
    vkCmdSetViewport( *_commandBuffer, 0, 1, &viewport );
    vkCmdSetScissor( *_commandBuffer, 0, 1, &scissor );
    vkCmdDraw( *_commandBuffer, 3, 1, 0, 0 );
    vkCmdEndRenderPass( *_commandBuffer );
    
    auto result = vkEndCommandBuffer( *_commandBuffer );
    CHECK_F( result == VK_SUCCESS, "Failed to record command buffer: %s", ResultMessage( result ) );

    _imageAvailableSemaphore = create<Semaphore>( _device );
    _renderFinishedSemaphore = create<Semaphore>( _device );
    _inFlightFence           = create<Fence>( _device );
}

VulkanViewport::~VulkanViewport()
{
}

} // namespace aer
