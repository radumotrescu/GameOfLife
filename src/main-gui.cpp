// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>

#include <ImplGameOfLife.h>
#include <ThreadUtils.h>
#include <thread>
#include <vector>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#endif

//static Semaphore t1(0);
//static Semaphore t2(1);
//static int count = 0;
//static int n = 2;

static Semaphore simSemaphore(0);
static Barrier bSixteen(16);
static Semaphore sixteenSemaphore(0);

namespace {
    void barrierSixteen(GameOfLife& gol, int compIdx, int done)
    {
        static Semaphore stateChangeMutex(1);

        sixteenSemaphore.wait(); // wait for the simulation thread to notify the start
       
        while (!done)
        {
            auto stateChange = gol.GenNextStateChanges<16>(compIdx);
            bSixteen.phase1();
            stateChangeMutex.wait();
            gol.DoStateChanges(stateChange);
            stateChangeMutex.notify();
            bSixteen.phase2();

            sixteenSemaphore.wait();
        }
    }

    void SimulationThreadCode(GameOfLife& gol, bool& done)
    {
        auto vecThread = std::vector<std::thread>();
        for (int i = 0; i < 16; i++)
        {
            vecThread.emplace_back(barrierSixteen, std::ref(gol), i, done);
        }
        while (!done)
        {
            simSemaphore.wait();

            // notify the 16 threads to start
            for (auto i = 0; i < 16; i++)
                sixteenSemaphore.notify();
        }

        for (int i = 0; i < sixteenSemaphore.GetCount(); i++)
            sixteenSemaphore.notify();
        for (auto& vec : vecThread)
        {
            vec.join();
        }
    }
}

//Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Game Of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2200, 1300, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    bool showGameOfLifeWindow = false;
    auto runSimulation = false;
    auto doNextIteration = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    auto boardSize = 200;
    auto gol = GameOfLife(boardSize);
    gol.InitBoardWithRandomData(5);
    //gol.SetInitialState({
    //    {1, 1},
    //    {2, 1},
    //    {3, 1},
    //    {31, 30},
    //    {31, 31},
    //    {31, 32},
    //    });

    auto threadVec = std::vector<std::thread>();

    auto green = IM_COL32(0, 200, 0, 255);
    auto red = IM_COL32(200, 0, 0, 255);

    bool done = false;
    // Simulation Thread
    auto simThread = std::thread(SimulationThreadCode, std::ref(gol), std::ref(done));

    // Main loop
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("Main Window");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Show Game of Life Window", &showGameOfLifeWindow);

            ImGui::End();
        }

        if (showGameOfLifeWindow)
        {
            static double refreshTime = 0.0;
            ImGui::SetNextWindowSize(ImVec2(1800, 1500));
            ImGui::Begin("Game Of Life Window", &showGameOfLifeWindow, ImGuiWindowFlags_HorizontalScrollbar);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            if (ImGui::Button("Quit"))
            {
                done = true;
            }
            if (ImGui::Button("Start simulation"))
            {
                runSimulation = true;
            }
            if (ImGui::Button("Stop simulation"))
            {
                runSimulation = false;
                refreshTime = 0.0;
            }

            ImGui::NewLine();
            //if (ImGui::Button("Next iteration"))
            //    doNextIteration = true;

            static int refreshRate = 2;
            ImGui::PushItemWidth(100);
            ImGui::InputInt("Refreshes per second", &refreshRate);
            ImGui::PopItemWidth();
            ImGui::Text("semaphore count %d", simSemaphore.GetCount());

            if (!runSimulation)
            {
                if (doNextIteration)
                {
                    doNextIteration = false;
                }
            }
            else
            {
                if (refreshTime == 0.0)
                {
                    refreshTime = ImGui::GetTime();
                }
                while (refreshTime < ImGui::GetTime())
                {
                    refreshTime += 1. / float(refreshRate);

                    // Let the simulation code run here
                    simSemaphore.notify();
                }
            }

            ImGui::NewLine();
            ImVec2 startPosition = ImGui::GetCursorScreenPos();      // this is the position at which the next ImGui object will be drawn, ImDrawList API uses screen coordinates!
            auto rectSize = ImVec2{ 5., 5. };

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            auto colorToDraw = green;
            for (int y = 0; y < boardSize; y++)
                for (int x = 0; x < boardSize; x++)
                {
                    auto rectStart = ImVec2(startPosition.x + x * rectSize.x, startPosition.y + y * rectSize.y);
                    colorToDraw = green;
                    if (gol[x][y])
                    {
                        colorToDraw = red;
                    }
                    drawList->AddRect(rectStart, ImVec2{ rectStart.x + rectSize.x, rectStart.y + rectSize.y }, colorToDraw);
                }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    simThread.join();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
