
///////////////////////////////////////////////////////////////////////////////
/// Copyright(c) Rodrigo Bento
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

///////////////////////////////////////////////////////////////////////////////

#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600

#define RENDER_DELAY 16

///////////////////////////////////////////////////////////////////////////////

typedef struct Application {
    SDL_Renderer* Renderer;
    SDL_Window* Window;
} Application;

///////////////////////////////////////////////////////////////////////////////

Application App;

///////////////////////////////////////////////////////////////////////////////

void Initialize(void);
void Cleanup(void);

void Startup(void);
void Run(void);
void Shutdown(void);

void Init_SDL(void);
void Read_Input(void);
void Pre_Render(void);
void Post_Render(void);
void Render(void);

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    Initialize();

    Startup();
    Run();
    Shutdown();

    Cleanup();

    return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

void Initialize(void)
{
    memset(&App, 0, sizeof(App));

    atexit(Cleanup);
}

///////////////////////////////////////////////////////////////////////////////

void Startup(void)
{
    Init_SDL();
}

///////////////////////////////////////////////////////////////////////////////

void Shutdown(void)
{
    SDL_DestroyRenderer(App.Renderer);
    SDL_DestroyWindow(App.Window);
}

///////////////////////////////////////////////////////////////////////////////

void Cleanup(void)
{
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////

void Run(void)
{
    while (1)
    {
        Pre_Render();

        Read_Input();

        Render();

        Post_Render();
    }
}

///////////////////////////////////////////////////////////////////////////////

void Init_SDL(void)
{
    int RendererFlags, WindowFlags;

    RendererFlags = SDL_RENDERER_ACCELERATED;

    WindowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    App.Window = SDL_CreateWindow("Starter", SDL_WINDOWPOS_UNDEFINED, 
                                             SDL_WINDOWPOS_UNDEFINED, 
                                             WINDOW_WIDTH, 
                                             WINDOW_HEIGHT, 
                                             WindowFlags);

    if (!App.Window)
    {
        printf("Unable to create %d x %d window: %s\n", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    App.Renderer = SDL_CreateRenderer(App.Window, -1, RendererFlags);

    if (!App.Renderer)
    {
        printf("Unable to create renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

///////////////////////////////////////////////////////////////////////////////

void Read_Input(void)
{
    SDL_Event Event;

    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;

        default:
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

void Pre_Render(void)
{
    SDL_SetRenderDrawColor(App.Renderer, 100, 200, 255, 255);
    SDL_RenderClear(App.Renderer);
}

///////////////////////////////////////////////////////////////////////////////

void Render(void)
{
    SDL_RenderPresent(App.Renderer);
}

///////////////////////////////////////////////////////////////////////////////

void Post_Render(void)
{
    SDL_Delay(RENDER_DELAY);
}

///////////////////////////////////////////////////////////////////////////////