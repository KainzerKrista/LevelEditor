#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <memory>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <filesystem>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_dx11.h"

#include "Renderer/Renderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshData.h"

#include "Assets/ObjectLoader.h"
#include "Assets/AssetManager.h"

#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"

#include "Editor/EditorCamera.h"

#include "Editor/Panels/HierarchyPanel.h"
#include "Editor/Panels/InspectorPanel.h"
#include "Editor/Panels/ContentPanel.h"
#include "Editor/Panels/ViewportPanel.h"

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

    constexpr std::uint32_t CubeIndices[] =
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

    // Create IMGUI
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    // Enable IMGUI Docking
    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Initialize IMGUI Backends
    if (!ImGui_ImplSDL3_InitForD3D(window))
    {
        std::cerr << "ERROR: Failed to initialize IMGUI SDL3 backend\n";
        return 1;
    }

    if (!ImGui_ImplDX11_Init(renderer.GetDevice(), renderer.GetDeviceContext()))
    {
        std::cerr << "ERROR: Failed to initialize IMGUI D3D11 backend\n";
        return 1;
    }

    // Create Editor Camera
    EditorCamera editorCamera;

    editorCamera.SetViewportSize(width, height);

    // Asset Manager
    AssetManager assetManager;

    auto cubeMesh = std::make_unique<Mesh>();

    if (!renderer.CreateMesh(*cubeMesh, CubeVertices, CubeIndices))
    {
        std::cerr << "ERROR: Failed to create Cube mesh\n";
        return 1;
    }

    if (!assetManager.RegisterMesh("Cube", std::move(cubeMesh)))
    {
        std::cerr << "ERROR: Failed to register Cube asset\n";

        return 1;
    }

    std::cout << "Working dir: "
        << std::filesystem::current_path()
        << '\n';

    // Discover object assets
    const std::filesystem::path modelsDirectory = "assets/models";

    if (std::filesystem::exists(modelsDirectory))
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(modelsDirectory))
        {
            // Ignore directories
            if (!entry.is_regular_file())
            {
                continue;
            }

            // .obj support
            if (entry.path().extension() != ".obj")
            {
                continue;
            }

            const std::string assetName = entry.path().stem().string();
            const std::string filePath = entry.path().string();

            std::cout
                << "Loading mesh assets: "
                << assetName
                << '\n';

            MeshData meshData;
            if (!ObjectLoader::Load(filePath, meshData))
            {
                std::cerr
                    << "ERROR: Failed to load mesh asset: "
                    << assetName
                    << '\n';

                continue;
            }

            auto mesh = std::make_unique<Mesh>();

            if (!renderer.CreateMesh(*mesh, meshData.vertices, meshData.indices))
            {
                std::cerr 
                    << "ERROR: Failed to create GPU mesh"
                    << assetName
                    << '\n';

                continue;
            }

            if (!assetManager.RegisterMesh(assetName, std::move(mesh)))
            {
                std::cerr
                    << "ERROR: Failed to register asset"
                    << assetName
                    << '\n';

                continue;
            }

            std::cout
                << "Loaded Mesh Asset: "
                << assetName
                << '\n';
        }
    }
    else
    {
        std::cout
            << "WARNING: Models dir does not exist!"
            << modelsDirectory
            << '\n';
    }

    // Cube Creation
    Scene scene;

    Entity& cube1 = scene.CreateEntity("Cube 1");
    MeshComponent& cube1Mesh = cube1.AddMeshComponent();
    cube1Mesh.assetName = "Cube";
    cube1Mesh.mesh = assetManager.GetMesh("Cube");

    cube1.GetTransform().position = glm::vec3(0.0f, 0.0f, 2.5f);
    cube1.GetTransform().rotation = glm::vec3(25.0f, 35.0f, 0.0f);

    Entity& cube2 = scene.CreateEntity("Cube 2");
    MeshComponent& cube2Mesh = cube2.AddMeshComponent();
    cube2Mesh.assetName = "Cube";
    cube2Mesh.mesh = assetManager.GetMesh("Cube");

    cube2.GetTransform().position = glm::vec3(1.2f, 0.0f, 3.5f);
    cube2.GetTransform().rotation = glm::vec3(25.0f, 35.0f, 0.0f);

    std::uint32_t selectedEntityID = 0;

    // Temporary mesh asset resolver
    auto meshResolver = [&assetManager](const std::string& assetName) ->Mesh*
    {
        return assetManager.GetMesh(assetName);
    };

    // Editor Panels
    HierarchyPanel hierarchyPanel;
    InspectorPanel inspectorPanel;
    ContentPanel contentPanel;
    ViewportPanel viewportPanel;

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
            // Bind IMGUI to SDL switch cases
            ImGui_ImplSDL3_ProcessEvent(&event);

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
                
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    if (event.button.button == SDL_BUTTON_RIGHT && viewportPanel.IsHovered())
                    {
                        editorCamera.SetLooking(true);

                        if (!SDL_SetWindowRelativeMouseMode(window, true))
                        {
                            std::cerr
                                << "ERROR: Failed to enable relative mouse mode:"
                                << SDL_GetError()
                                << '\n';
                        };

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
                    if (editorCamera.IsLooking() && viewportPanel.IsHovered())
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

        // Process Events for IMGUI
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Editor UI
        ImGui::DockSpaceOverViewport();

        // Hierarchy Content
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {

                if (ImGui::MenuItem("New Scene"))
                {
                    scene.Clear();
                    selectedEntityID = 0;

                }

                if (ImGui::MenuItem("Open Scene"))
                {
                    if (SceneSerializer::Load(scene, "scene.json", meshResolver))
                    {
                        selectedEntityID = 0;
                        std::cout << "Scene Loaded Successfully";
                    }
                }

                if (ImGui::MenuItem("Save Scene"))
                {
                    if (SceneSerializer::Save(scene, "scene.json"))
                    {
                        std::cout << "Scene saved successfully\n";
                    }
                }


                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    running = false;
                }

                ImGui::MenuItem("Hierarchy");
                ImGui::MenuItem("Inspector");
                
                // Content Browser
                ImGui::MenuItem("Content Browser");

                ImGui::EndMenu();

            }

            ImGui::EndMainMenuBar();
        }

        // Scene Hierarchy Content
        hierarchyPanel.Draw(scene, selectedEntityID);

        // Inspector Content
        inspectorPanel.Draw(scene, assetManager, selectedEntityID);

        // Content Browser Content
        contentPanel.Draw(assetManager);

        // Viewport Panel Content
        viewportPanel.Draw(renderer, editorCamera);

        // Camera Matrices
        const glm::mat4 view = editorCamera.GetViewMatrix();
        const glm::mat4 projection = editorCamera.GetProjectionMatrix();

        // Rendering
        renderer.BeginViewportFrame(0.09f, 0.08f, 0.11f, 1.0f);
      
        // Gets all scene entities to render
        for (const Entity& entity : scene.GetEntities())
        {
            // Checks if Entity has a valid mesh component before drawing it, otherwise skip
            const MeshComponent* meshComponent = entity.GetMeshComponent();

            if (meshComponent == nullptr || meshComponent->mesh == nullptr)
            {
                continue;
            }

            const glm::mat4  model = entity.GetTransform().GetTransform();

            renderer.DrawMesh(*meshComponent->mesh, model, view, projection);

        }

        renderer.EndViewportFrame();

        // Render the editor window (IMGUI) on top of the scene
        renderer.BeginFrame(0.08f, 0.09f, 0.11f, 1.0f);
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present frames
        renderer.EndFrame();        
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    assetManager.Shutdown();
    renderer.Shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}