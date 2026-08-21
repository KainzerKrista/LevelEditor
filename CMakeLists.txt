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
    assets/shaders/Triangle.hlsl
 "src/Mesh.h" "src/Mesh.cpp")

set_source_files_properties(
    assets/shaders/Triangle.hlsl
    PROPERTIES
        HEADER_FILE_ONLY TRUE
)

target_link_libraries(LevelEditor
    PRIVATE
        SDL3::SDL3
        glm::glm
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