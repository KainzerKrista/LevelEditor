cmake_minimum_required(VERSION 3.16)
project(LevelEditor
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)

add_executable(LevelEditor
    src/main.cpp
    src/Renderer.cpp 
    src/Mesh.cpp
    src/EditorCamera.cpp
    src/Entity.cpp
    src/Scene.cpp
    src/TransformComponent.h
    assets/shaders/Triangle.hlsl
 "src/Mesh.h" "src/Mesh.cpp" "src/EditorCamera.h" "src/EditorCamera.cpp")
 
set_source_files_properties(
    assets/shaders/Triangle.hlsl
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp

    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui
    PUBLIC
        external/imgui
        external/imgui/backends
        external/SDL/include
)

target_link_libraries(imgui 
    PUBLIC
        SDL3::SDL3
)

target_link_libraries(LevelEditor
    PRIVATE
        SDL3::SDL3
        glm::glm
        imgui
)

if(WIN32)
    target_link_libraries(LevelEditor
    PRIVATE
            d3d11
            dxgi
            d3dcompiler
    )

    target_compile_definitions(LevelEditor
        PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
    )

    add_custom_command(
        TARGET LevelEditor POST_BUILD 

         # Copy SDL3.dll beside LevelEditor.exe
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:LevelEditor>

        # Create the runtime shader directory
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:LevelEditor>/assets/shaders"

        # Copy our HLSL shader
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/Triangle.hlsl"
            "$<TARGET_FILE_DIR:LevelEditor>/assets/shaders/Triangle.hlsl"

        VERBATIM
    )

endif()