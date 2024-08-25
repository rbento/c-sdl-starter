
///////////////////////////////////////////////////////////////////////////////
/// Copyright(c) Rodrigo Bento
///////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>

///////////////////////////////////////////////////////////////////////////////

#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  600

#define RENDER_DELAY 16

#define MAX_PATH_SIZE 256
#define MAX_RESOURCE_SLOTS 8

///////////////////////////////////////////////////////////////////////////////

typedef struct App_Context
{
    SDL_Renderer* Renderer;
    SDL_Window* Window;
    Uint64 ShouldQuit;
} App_Context;

typedef struct App_Resource
{
    SDL_Surface* Surfaces[MAX_RESOURCE_SLOTS];
    SDL_Texture* Textures[MAX_RESOURCE_SLOTS];
    unsigned long Slot;
} App_Resource;

typedef struct App_Tick
{
    Uint64 Now;
    Uint64 Last;
    double DeltaTime;
} App_Tick;

///////////////////////////////////////////////////////////////////////////////

App_Context App;
App_Resource Resource;
App_Tick Tick;

///////////////////////////////////////////////////////////////////////////////

void Initialize(void);
void Cleanup(void);

void Startup(void);
void Run(void);
void Shutdown(void);

void Free_Context(void);
void Free_Resources(void);

void Init_SDL(void);
void Init_SDL_Image(void);

void Init_Tick(void);
void Update_Tick(void);

void Read_Input(void);

void Pre_Render(void);
void Render(void);
void Post_Render(void);

unsigned long Load_Texture(char* Path);
void Draw_Texture_At(unsigned long Slot, int X, int Y, int Width, int Height);

void Load_Content(void);
void Simulate(double DeltaTime);

///////////////////////////////////////////////////////////////////////////////

int main(void)
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
    memset(&Resource, 0, sizeof(Resource));
    memset(&Tick, 0, sizeof(Tick));

    atexit(Cleanup);
}

///////////////////////////////////////////////////////////////////////////////

void Startup(void)
{
    Init_SDL();
    Init_SDL_Image();

    Load_Content();
}

///////////////////////////////////////////////////////////////////////////////

void Shutdown(void)
{
    Free_Context();
    Free_Resources();
}

///////////////////////////////////////////////////////////////////////////////

void Cleanup(void)
{
    IMG_Quit();
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////

void Run(void)
{
    Init_Tick();

    while (!App.ShouldQuit)
    {
        Update_Tick();

        Pre_Render();

        Read_Input();

        Simulate(Tick.DeltaTime);

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

void Init_SDL_Image(void)
{
    IMG_Init(IMG_INIT_PNG);
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
            App.ShouldQuit = 1;
            break;

        case SDL_KEYDOWN:
            switch (Event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                App.ShouldQuit = 1;
                break;
            }
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

void Init_Tick(void)
{
    Tick.Now = SDL_GetPerformanceCounter();
    Tick.Last = 0;
    Tick.DeltaTime = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Update_Tick(void)
{
    Tick.Last = Tick.Now;
    Tick.Now = SDL_GetPerformanceCounter();
    Tick.DeltaTime = (double)((Tick.Now - Tick.Last) * 1000 / (double)SDL_GetPerformanceFrequency());
}

///////////////////////////////////////////////////////////////////////////////

unsigned long Load_Texture(char* Path)
{
    assert(Path);

    if (Resource.Slot >= MAX_RESOURCE_SLOTS)
    {
        printf("Max resource slots exceeded. Actual: %lu Max: %lu\n", Resource.Slot, MAX_RESOURCE_SLOTS);
        exit(EXIT_FAILURE);
    }

    char RelativePath[MAX_PATH_SIZE];
    strcpy_s(RelativePath, sizeof RelativePath, SDL_GetBasePath());
    strcat_s(RelativePath, sizeof RelativePath, Path);

    Resource.Surfaces[Resource.Slot] = IMG_Load(RelativePath);

    if (!Resource.Surfaces[Resource.Slot])
    {
        printf("Unable to load surface from: %s \n", Path);
        printf("Error: %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    Resource.Textures[Resource.Slot] = SDL_CreateTextureFromSurface(App.Renderer, Resource.Surfaces[Resource.Slot]);

    if (!Resource.Textures[Resource.Slot])
    {
        printf("Unable to load texture from: %s \n", Path);
        printf("Error: %s \n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return Resource.Slot++;
}

///////////////////////////////////////////////////////////////////////////////

void Free_Context(void)
{
    SDL_DestroyRenderer(App.Renderer);
    SDL_DestroyWindow(App.Window);
}

///////////////////////////////////////////////////////////////////////////////

void Free_Resources(void)
{
    for (unsigned long Slot = 0; Slot < MAX_RESOURCE_SLOTS; Slot++)
    {
        SDL_DestroyTexture(Resource.Textures[Slot]);
        SDL_FreeSurface(Resource.Surfaces[Slot]);
    }
}

///////////////////////////////////////////////////////////////////////////////

void Draw_Texture_At(unsigned long Texture, int X, int Y, int Width, int Height)
{
    assert(Texture >= 0 && Texture < MAX_RESOURCE_SLOTS);

    SDL_Rect DestRect = { X, Y, Width, Height };
    SDL_RenderCopy(App.Renderer, Resource.Textures[Texture], NULL, &DestRect);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long Roboto;

void Load_Content(void)
{
    Roboto = Load_Texture("Roboto.png");
}

///////////////////////////////////////////////////////////////////////////////

void Simulate(double DeltaTime)
{
    Draw_Texture_At(Roboto, 20, 20, 567, 556);
}

///////////////////////////////////////////////////////////////////////////////
