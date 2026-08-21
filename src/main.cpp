#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cstdint>

#include <iostream>

#include "Renderer.h"

namespace
{
    // Test Cube
    constexpr Vertex CubeVertices[] =
    {
        // Front 
        {
            {-0.5f, 0.5f, -0.5f},
            {1.0f, 0.0f, 0.0f, 1.0f}
        },

        {
            {0.5f, 0.5f, -0.5f},
            {0.0f, 1.0f, 0.0f, 1.0f}
        },

        {
            {0.5f, -0.5f, -0.5f},
            {0.0f, 0.0f, 1.0f, 1.0f}
        },
        {
            {-0.5f, -0.5f, -0.5f},
            {1.0f, 1.0f, 0.0f, 1.0f}
        },

        // Back
        {
            {-0.5f, 0.5f, 0.5f},
            {1.0f, 0.0f, 1.0f, 1.0f}
        },

        {
            {0.5f, 0.5f, 0.5f},
            {0.0f, 1.0f, 1.0f, 1.0f}
        },

        {
            {0.5f, -0.5f, 0.5f},
            {1.0f, 1.0f, 1.0f, 1.0f}
        },
        {
            {-0.5f, -0.5f, 0.5f},
            {0.3f, 0.3f, 0.3f, 1.0f}
        }

    };

    constexpr std::uint16_t CubeIndices[] =
    {
        // Front
        0, 1, 2,
        0, 2, 3,

        // Back
        5, 4, 7,
        5, 7, 6,

        // Left
        4, 0, 3,
        4, 3, 7,

        // Right
        1, 5, 6,
        1, 6, 2,

        // Top
        4, 5, 1,
        4, 1, 0,

        // Bottom
        3, 2, 6,
        3, 6, 7
    };
}

int main(int argc, char* argv[])
{
    // Initialise SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr
            << "Failed to initialise SDL: "
            << SDL_GetError()
            << '\n';

        return 1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("Level Editor", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (window == nullptr)
    {
        std::cerr
            << "Failed to create SDL window: "
            << SDL_GetError()
            << '\n';

        SDL_Quit();
        return 1;
    }

    // Get native Windows HWND
    SDL_PropertiesID properties = SDL_GetWindowProperties(window);

    HWND windowHandle = static_cast<HWND>(SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    if (windowHandle == nullptr)
    {
        std::cerr
            << "Failed to retrieve Windows HWND from SDL.\n";

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    // Get actual drawable pixel size
    int width = 0;
    int height = 0;

    SDL_GetWindowSizeInPixels(window, &width, &height);

    // Create Renderer window
    Renderer renderer;
    
    if (!renderer.Initialize(windowHandle, width, height))
    {
        std::cerr << "Failed to initialize renderer\n";

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    // Create Cube Mesh
    Mesh cubeMesh;

    if (!renderer.CreateMesh(cubeMesh, CubeVertices, CubeIndices))
    {
        std::cerr << "ERROR: Failed to create cube mesh\n";

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    // Cube Transform
    glm::mat4 cubeTransform = glm::mat4(1.0f);

    cubeTransform = glm::translate(cubeTransform, glm::vec3(0.0f, 0.0f, 2.5f));
    cubeTransform = glm::rotate(cubeTransform, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cubeTransform = glm::rotate(cubeTransform, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 cubeTransform2 = glm::mat4(1.0f);

    cubeTransform2 = glm::translate(cubeTransform2, glm::vec3(1.2f, 0.0f, 3.5f));
    cubeTransform2 = glm::rotate(cubeTransform2, glm::radians(25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cubeTransform2 = glm::rotate(cubeTransform2, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Application loop
    bool running = true;

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    running = false;
                    break;
                }

                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                {
                    int newWidth = 0;
                    int newHeight = 0;

                    SDL_GetWindowSizeInPixels(window, &newWidth, &newHeight);

                    renderer.Resize(newWidth, newHeight);

                    break;
                }
            }
        }
        // Rendering
        renderer.BeginFrame(0.08f, 0.09f, 0.11f, 1.0f);

        // TODO: Draw Scenes functions are added here later
        renderer.DrawMesh(cubeMesh, cubeTransform);
        renderer.DrawMesh(cubeMesh, cubeTransform2);
        renderer.EndFrame();
    }
    
    // Render Cleanup
    cubeMesh.Shutdown();
    renderer.Shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}