#pragma region imgui stuff
// Dear ImGui: standalone example application for SDL3 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#define GL_GLEXT_PROTOTYPES
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/glew.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif
#pragma endregion
#include <iostream>
#include <fstream>
#include "Render/Render.h"
#include "Render/Mesh.h"
#include "def.h"
#include "Render/Shader/Shader.h"
#include "Render/Gizmos.h"
#include "Geometry.h"
#include "Editor/Handles.h"
#include "Trace.h"

namespace im = ImGui;

//actual size of the window
int windowWidth, windowHeight;
//event related
int hasEvent = 0;
//render
constexpr int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;
Renderer renderer;
Mesh mesh_monkey, mesh_cube, mesh_quad, mesh_invCube;
Shader_BlinnPhong shader_blinnPhong;
Shader_Unlit_Texture shader_unlit_tex, shader_skybox;
Texture tex_test, tex_skyBox;
RenderTexture rt;

void gui() {
    if (im::Begin("Config")) {
        renderer.Gui();
        shader_blinnPhong.OnGui();
		mesh_monkey.Gui();
        mesh_cube.Gui();
        mesh_quad.Gui();
        mesh_invCube.Gui();
    }
    im::End();
    Trace::Gui();
}
void init() {
    Trace::Begin("opengl init");
    glewInit();
    renderer.Init(windowWidth, windowHeight);
    Trace::End();
    //load textures
    Trace::Begin("load textures");
    tex_test.LoadFromFile("Resources\\Texture\\tex.png");
    tex_skyBox.LoadFromFile("Resources\\Texture\\skybox.png");
    Trace::End();
    //render textures
    rt.Create(windowWidth, windowHeight);
    //load shaders
    Trace::Begin("load shaders");
    shader_blinnPhong.Load();
    shader_blinnPhong.tex = &tex_test.renderTex;
    shader_unlit_tex.Load();
    shader_unlit_tex.texture = &rt;
    shader_skybox.Load();
    shader_skybox.texture = &tex_skyBox.renderTex;
    Trace::End();
    //load models
    Trace::Begin("load models");
    mesh_monkey.LoadFromFile("Resources\\Mesh\\monkey.obj");
    mesh_monkey.transform.position = glm::vec3(0, 0, 3);
    mesh_monkey.transform.SetRotation(glm::vec3(0, 0, 0));
    mesh_monkey.shader = &shader_blinnPhong;
    mesh_cube.LoadFromFile("Resources\\Mesh\\torus.obj");
    mesh_cube.transform.position = glm::vec3(3, 0, 2);
    mesh_cube.transform.SetRotation(glm::vec3(0, 0, 0));
    mesh_cube.shader = &shader_blinnPhong;
    mesh_quad.LoadFromFile("Resources\\Mesh\\quad.obj");
    mesh_quad.transform.position = glm::vec3(-3, 0, 1);
    mesh_quad.transform.SetRotation(glm::vec3(0, 0, 0));
    mesh_quad.shader = &shader_unlit_tex;
    mesh_quad.name = "quad";
    mesh_invCube.LoadFromFile("Resources\\Mesh\\invcube.obj");
    mesh_invCube.transform.position = glm::vec3(0, 0, 0);
    mesh_invCube.transform.scale = glm::vec3(100, 100, 100);
    mesh_invCube.transform.SetRotation(glm::vec3(0, 0, 0));
    mesh_invCube.shader = &shader_skybox;
    mesh_invCube.name = "inv cube";
    Trace::End();
    //setup RenderObjects callback
    renderer.RenderObjects = []() {
		renderer.SetGLOpaque();
		renderer.RenderMesh(mesh_monkey);
		renderer.RenderMesh(mesh_cube);
        renderer.RenderMesh(mesh_invCube);
        renderer.RenderMesh(mesh_quad);
        renderer.RenderGizmos(Gizmos::Get());
        };
    renderer.PostProcessing = []() {
        renderer.Blit(&rt, &renderer.frameTexture, Shader_PP::Get());
        rt.Swap(renderer.frameTexture);
        };
}
void update() {
    Trace::Begin("update");
    const bool* keys = SDL_GetKeyboardState(NULL);
    int oldHasEvent = hasEvent;
    hasEvent = 2;
    //camera movement
    const constexpr float camMoveSpd = .1f;
    if (keys[SDL_SCANCODE_A])
        renderer.camera.transform.position -= renderer.camera.transform.Right() * camMoveSpd;
    else if (keys[SDL_SCANCODE_D])
        renderer.camera.transform.position += renderer.camera.transform.Right() * camMoveSpd;
    else if (keys[SDL_SCANCODE_S])
        renderer.camera.transform.position += renderer.camera.transform.Forward() * camMoveSpd;
    else if (keys[SDL_SCANCODE_W])
        renderer.camera.transform.position -= renderer.camera.transform.Forward() * camMoveSpd;
    else if (keys[SDL_SCANCODE_Q])
        renderer.camera.transform.position -= renderer.camera.transform.Up() * camMoveSpd;
    else if (keys[SDL_SCANCODE_E])
        renderer.camera.transform.position += renderer.camera.transform.Up() * camMoveSpd;
    else
        hasEvent = oldHasEvent;
    Gizmos::BeginFrame();
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Handles::Axis(mesh_monkey.transform.position);
    Handles::Axis(mesh_cube.transform.position);
    Handles::EndFrame();
    Trace::End();
}
void handle_event(SDL_Event& event) {
    Trace::Begin("event handling");
    Handles::HandleEvent(event);
    switch (event.type) {
    case SDL_EVENT_MOUSE_WHEEL:
        renderer.camera.transform.position-=renderer.camera.transform.Forward()*event.wheel.y;
        break;
    case SDL_EVENT_MOUSE_MOTION:
        if (SDL_GetMouseState(0, 0) & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) {
            Transform& transform = renderer.camera.transform;
            glm::quat rotY=glm::angleAxis(glm::radians(-event.motion.xrel), Transform::up);
            glm::quat rotX=glm::angleAxis(glm::radians(-event.motion.yrel), Transform::right);
            transform.rotation = rotY * transform.rotation * rotX;
        }
        break;
    }
    Trace::End();
}
void draw() {
    Trace::Begin("render");
    renderer.Render();
    Trace::End();
    renderer.RenderDepthTexture();
}
Renderer& Renderer::Get() { return renderer; }

// Main code
int main(int, char**)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", (int)(SCREEN_WIDTH * main_scale), (int)(SCREEN_HEIGHT * main_scale), window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);
    init();

    // Main loop
    bool done = false;
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!done)
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            hasEvent = 3;
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            else if(!(io.WantCaptureKeyboard||io.WantCaptureMouse))
                handle_event(event);
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        if (hasEvent) {
            --hasEvent;
            Trace::Begin("Frame");
            update();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
            //if (show_demo_window)
                //ImGui::ShowDemoWindow(&show_demo_window);

            gui();

            // Rendering
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            draw();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            Trace::End();
        }
        else {
            SDL_Delay(10);
            continue;
        }
        SDL_GL_SwapWindow(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
