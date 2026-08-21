#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cstdint>
#include <chrono>
#include <iostream>

#include "Renderer.h"
#include "EditorCamera.h"

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

    // Create Editor Camera
    EditorCamera editorCamera;

    editorCamera.SetViewportSize(width, height);

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

    // Application loop to run per second
    auto previousTime = std::chrono::steady_clock::now();
    bool running = true;

    while (running)
    {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;

        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }

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

                    editorCamera.SetViewportSize(newWidth, newHeight);

                    break;
                }
                
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        editorCamera.SetLooking(true);

                        if (!SDL_SetWindowRelativeMouseMode(window, true))
                        {
                            std::cerr
                                << "ERROR: Failed to enable relative mouse mode:"
                                << SDL_GetError()
                                << '\n';
                        }

                    }

                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        editorCamera.SetLooking(false);

                        if (!SDL_SetWindowRelativeMouseMode(window, false))
                        {
                            std::cerr
                                << "ERROR: Failed to disable relative mouse mode:"
                                << SDL_GetError()
                                << '\n';
                        }

                    }

                    break;
                }

                // Exposes xrel and yrel as relative mouse movements for camera rotation
                case SDL_EVENT_MOUSE_MOTION:
                {
                    if (editorCamera.IsLooking())
                    {
                        editorCamera.Rotate(event.motion.xrel, event.motion.yrel);
                    }

                    break;
                }
            }
        }

        // Keyboard Binding
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);

        if (editorCamera.IsLooking())
        {
            float forwardInput = 0.0f;
            float rightInput = 0.0f;
            float upInput = 0.0f;

            // Forward
            if (keyboardState[SDL_SCANCODE_W])
            {
                forwardInput += 1.0f;
            }

            // Backwards
            if (keyboardState[SDL_SCANCODE_S])
            {
                forwardInput -= 1.0f;
            }

            // Right
            if (keyboardState[SDL_SCANCODE_D])
            {
                rightInput += 1.0f;
            }

            // Left
            if (keyboardState[SDL_SCANCODE_A])
            {
                rightInput -= 1.0f;
            }

            // Up
            if (keyboardState[SDL_SCANCODE_E])
            {
                upInput += 1.0f;
            }

            // Down
            if (keyboardState[SDL_SCANCODE_Q])
            {
                upInput -= 1.0f;
            }

            // Boost
            bool boost = keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];

            // Move camera
            editorCamera.Move(forwardInput, rightInput, upInput, deltaTime, boost);
        }

        // Camera Matrixes
        const glm::mat4 view = editorCamera.GetViewMatrix();
        const glm::mat4 projection = editorCamera.GetProjectionMatrix();

        // Rendering
        renderer.BeginFrame(0.08f, 0.09f, 0.11f, 1.0f);

        // TODO: Draw Scenes functions are added here later


        renderer.DrawMesh(cubeMesh, cubeTransform, view, projection);
        renderer.DrawMesh(cubeMesh, cubeTransform2, view, projection);
        renderer.EndFrame();
    }
    
    // Render Cleanup
    cubeMesh.Shutdown();
    renderer.Shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}