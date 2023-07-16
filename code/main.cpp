#include "SDL.h"
#include <stdio.h>
#include "constants.h"
#include "types.c"

int game_is_running = 0;
SDL_Surface* backbuffer = NULL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Rect drag_rect;
SDL_Texture* backbufferTexture = NULL;

bool is_selecting = false;

int last_frame_time = 0;

RGB rgb;

int initialize_window()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        fprintf(stderr, "ERROR Initializing SDL.\n");
        return FALSE;
    }



    window = SDL_CreateWindow(NULL,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_BORDERLESS);
    if (!window)
    {
        fprintf(stderr, "Error creating sdl window.\n");
        return FALSE;
    }

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
    }

    renderer = SDL_CreateRenderer(window,
                                  -1,
                                  0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating the rednerer");
        return FALSE;
    }
    return TRUE;
}

void TestConvertHexToRgb(const char* input )
{
    if (input[0] == '#')
    {
        input++;
    }
    sscanf(input, "%02x%02x%02x", &rgb.r, &rgb.g, &rgb.b);
}

void setup()
{
    TestConvertHexToRgb(DRAG_RECT_HEX);
}

void process_input()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        game_is_running = 0;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            game_is_running = 0;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            is_selecting = true;
            drag_rect.x = event.button.x;
            drag_rect.y = event.button.y;
        }
        break;
    case SDL_MOUSEMOTION:
        if (is_selecting)
        {
            drag_rect.w = event.motion.x - drag_rect.x;
            drag_rect.h = event.motion.y - drag_rect.y;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            is_selecting = false;
            drag_rect.h = 0;
            drag_rect.w = 0;
        }
        break;
    }
}

void update()
{
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    last_frame_time = SDL_GetTicks();
}

// Function to create the backbuffer with the same dimensions as the window
void CreateBackbuffer(SDL_Renderer* renderer, int window_width, int window_height)
{
    if (backbuffer != NULL)
    {
        SDL_FreeSurface(backbuffer);
    }

    backbuffer = SDL_CreateRGBSurface(0, window_width, window_height, 32, 0, 0, 0, 0);
}

void RenderWithBackbuffer(SDL_Renderer* renderer, int window_width, int window_height)
{
    // Set the draw color for the backbuffer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Clear the backbuffer
    SDL_RenderClear(renderer);

    // Set the draw color for the green square
    SDL_SetRenderDrawColor(renderer, rgb.r, rgb.g, rgb.b, 255);

    // Render onto the backbuffer
    if (is_selecting)
    {
        SDL_RenderFillRect(renderer, &drag_rect);
    }

    // Create a texture from the backbuffer surface
    backbufferTexture = SDL_CreateTextureFromSurface(renderer, backbuffer);

    // Copy the backbuffer onto the front buffer
    SDL_RenderReadPixels(renderer, nullptr, backbuffer->format->format, backbuffer->pixels, backbuffer->pitch);

    // Render the backbuffer texture onto the screen
    SDL_RenderCopy(renderer, backbufferTexture, nullptr, nullptr);

    // Present the renderer (swap the front and back buffers)
    SDL_RenderPresent(renderer);

    // Clean up
    SDL_DestroyTexture(backbufferTexture);
}

void destroy_app()
{
    if (backbufferTexture) {
        SDL_DestroyTexture(backbufferTexture);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    game_is_running = initialize_window();

    CreateBackbuffer(renderer,WINDOW_WIDTH,WINDOW_HEIGHT);

    setup();

    while (game_is_running)
    {
        process_input();
        update();
        RenderWithBackbuffer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    destroy_app();
    return 0;
}