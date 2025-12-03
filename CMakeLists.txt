# cmake -------------------------------------------------------------------------------------------
cmake_minimum_required( VERSION 3.20 )

project( aer-graphics VERSION 0.1.0 LANGUAGES CXX )

include(FetchContent)

set( DOWNLOADS_DIR ${PROJECT_BINARY_DIR}/downloads )
set( COMPONENTS_DIR ${PROJECT_BINARY_DIR}/components )
set( FETCHCONTENT_BASE_DIR ${COMPONENTS_DIR} )

# dependencies ------------------------------------------------------------------------------------
if( NOT TARGET base )
    FetchContent_Declare( aer_base
        GIT_REPOSITORY  https://github.com/AERwaevs/base.git
        GIT_TAG         main
        GIT_PROGRESS    TRUE
    )
    FetchContent_MakeAvailable( aer_base )
endif()

add_subdirectory( "lib/vulkan" )
# Graphics ----------------------------------------------------------------------------------
set( LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib )
set( INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc )
set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

set( HEADERS
    ${INC_DIR}/Graphics/Viewport.h
    ${INC_DIR}/Graphics/Window.h
    ${INC_DIR}/Events/WindowEvents.h

    ${INC_DIR}/Graphics/VulkanViewport.h
    ${INC_DIR}/Graphics/VulkanRenderer.h
    ${INC_DIR}/Graphics/RenderGraph.h
    
)

set( SOURCES
    ${SRC_DIR}/VulkanViewport.cpp
    ${SRC_DIR}/VulkanRenderer.cpp
    ${SRC_DIR}/RenderGraph.cpp
    
    ${SRC_DIR}/Renderer.cpp
    ${SRC_DIR}/Viewport.cpp
    ${SRC_DIR}/Window.cpp
)

# gfx module --------------------------------------------------------------------------------------
add_library( graphics SHARED ${SOURCES} )
target_include_directories( graphics PUBLIC
    $<BUILD_INTERFACE:${INC_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries( graphics
    PUBLIC
        aer::base
        aer::vulkan
        #$<$<BOOL:${WIN32}>:win32>
        #$<$<BOOL:${LINUX}>:linux>
        $<$<BOOL:${ANDROID}>:android>
        $<$<BOOL:${IOS}>:ios>
        $<$<AND:$<BOOL:${APPLE}>,$<NOT:$<BOOL:${IOS}>>>:macos>
)

set_target_properties( graphics PROPERTIES
    CXX_STANDARD                    23
    CXX_STANDARD_REQUIRED           ON
    CXX_EXTENSIONS                  OFF
    CMAKE_POSITION_INDEPENDENT_CODE ON
)

add_library( aer::graphics ALIAS graphics )