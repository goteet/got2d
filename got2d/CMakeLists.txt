cmake_minimum_required(VERSION 3.12)

set(GOT2D_INCLUDE_FILES
	include/g2dconfig.h
	include/g2dobject.h
	include/g2dengine.h
	include/g2dinput.h
	include/g2dmessage.h
	include/g2drender.h
	include/g2dscene.h
)

set(GOT2D_SOURCE_FILES
	source/g2dobject.cpp
	source/g2dengine.cpp
	source/g2dscene.cpp
	source/g2drender.cpp
	source/engine.h
	source/engine.cpp
	source/system_blackboard.h
	source/system_blackboard.cpp
	source/inner_utility.h
	source/scope_utility.h
)

set(GOT2D_SOURCE_INPUT_FILES
	source/input/input.h
	source/input/input.cpp
	source/input/message.cpp
)

if(MSVC)
list(APPEND GOT2D_SOURCE_INPUT_FILES 
	source/input/message_win32.cpp
)
endif(MSVC)

set(GOT2D_SOURCE_RENDER_FILES
	source/render/render_system.h
	source/render/render_system.cpp
	source/render/material.h
	source/render/material.cpp
	source/render/pass.h
	source/render/pass.cpp
	source/render/texture.h
	source/render/texture.cpp
	source/render/mesh.h
	source/render/mesh.cpp
	source/render/shader.h
	source/render/shader.cpp	
)

set(GOT2D_SOURCE_SCENE_FILES
	source/scene/scene.h
	source/scene/scene.cpp
	source/scene/scene_node.h
	source/scene/scene_node.cpp
	source/scene/scene_node_container.h
	source/scene/scene_node_container.cpp
	source/scene/component_container.h
	source/scene/component_container.cpp
	source/scene/quad.h
	source/scene/quad.cpp
	source/scene/camera.h
	source/scene/camera.cpp
	source/scene/transform.h
	source/scene/transform.cpp
	source/scene/spatial_graph.h
	source/scene/spatial_graph.cpp
)

set(GOT2D_RHI_INCLUDE_FILES
	RHI/RHI.h
)

if(MSVC)
list(APPEND GOT2D_RHI_INCLUDE_FILES 
	RHI/dx11/inner_RHI.h
	RHI/dx11/dx11_enum.h
	RHI/dx11/RHI.cpp
	RHI/dx11/rhi_device.cpp
	RHI/dx11/rhi_context.cpp
	RHI/dx11/rhi_swapchain.cpp
	RHI/dx11/rhi_shader.cpp
)
source_group(RHI_DX11 FILES ${GOT2D_RHI_INCLUDE_FILES})
else()
source_group(RHI FILES ${GOT2D_RHI_INCLUDE_FILES})
endif(MSVC)

source_group(include FILES ${GOT2D_INCLUDE_FILES})
source_group(source FILES ${GOT2D_SOURCE_FILES})
source_group(source\\input FILES ${GOT2D_SOURCE_INPUT_FILES})
source_group(source\\render FILES ${GOT2D_SOURCE_RENDER_FILES})
source_group(source\\scene FILES ${GOT2D_SOURCE_SCENE_FILES})

list(APPEND GOT2D_SOURCE_FILES 
	${GOT2D_SOURCE_INPUT_FILES}
	${GOT2D_SOURCE_RENDER_FILES}
	${GOT2D_SOURCE_SCENE_FILES}
	${GOT2D_RHI_INCLUDE_FILES}
)

add_library(got2d SHARED 
    ${GOT2D_INCLUDE_FILES}
    ${GOT2D_SOURCE_FILES}
)

target_include_directories(got2d PUBLIC include)

set_target_properties(got2d PROPERTIES
    COMPILE_DEFINITIONS GOT2D_EXPORTS
)

target_link_libraries(got2d cxx res)
if(MSVC)
    target_link_libraries(got2d
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
    )
endif(MSVC)