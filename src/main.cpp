#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Windows.h>

#include <iostream>

#include "Renderer.h"

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

    // Create Renderer
    Renderer renderer;
    
    // Check if window can be created first
    if (!renderer.Initialize(windowHandle, width, height))
    {
        std::cerr << "Failed to initialize renderer\n";

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

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
        renderer.DrawTriangle();
        renderer.EndFrame();
    }
    
    // Render Cleanup
    renderer.Shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}