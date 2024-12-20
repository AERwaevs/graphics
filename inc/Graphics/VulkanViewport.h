#pragma once

#include <Graphics/Viewport.h>
#include <Graphics/Window.h>

#include <vk/Instance.h>
#include <vk/Surface.h>
#include <vk/LogicalDevice.h>
#include <vk/Swapchain.h>
#include <vk/Context.h>
#include <vk/PipelineLayout.h>
#include <vk/GraphicsPipeline.h>
#include <vk/Framebuffer.h>
#include <vk/CommandPool.h>
#include <vk/CommandBuffer.h>
#include <vk/Semaphore.h>
#include <vk/Fence.h>

#include <vk/state/Image.h>
#include <vk/state/ImageView.h>

#include "VulkanRenderer.h"

namespace aer
{
    class VulkanViewport : public Viewport
    {
        friend class VulkanRenderer;

    public:
        VulkanViewport( Window* window );
        ~VulkanViewport();
    private:
        ref_ptr<vk::Instance>           _instance;
        ref_ptr<vk::Surface>            _surface;
        ref_ptr<vk::PhysicalDevice>     _physical_device;
        ref_ptr<vk::Device>             _device;
        vk::SwapchainPreferences        _swapchain_prefs{};
        ref_ptr<vk::Swapchain>          _swapchain;
        ref_ptr<vk::Context>            _context;
        ref_ptr<vk::PipelineLayout>     _pipelineLayout;
        ref_ptr<vk::GraphicsPipeline>   _graphicsPipeline;
        vk::Framebuffers                _framebuffers;
        ref_ptr<vk::CommandPool>        _commandPool;
        ref_ptr<vk::CommandBuffer>      _commandBuffer;
        ref_ptr<vk::Semaphore>          _imageAvailableSemaphore;
        ref_ptr<vk::Semaphore>          _renderFinishedSemaphore;
        ref_ptr<vk::Fence>              _inFlightFence;
        
        // TODO - handle multisampling
        //ref_ptr<vk::Image>              _multisampleImage;
        //ref_ptr<vk::Image>              _multisampleDepthImage;
        // TODO - handle depth
        //ref_ptr<vk::Image>              _depthImage;
        //ref_ptr<vk::ImageView>          _depthImageView;

    };
} // namespace aer
