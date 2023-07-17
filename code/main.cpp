#include "SDL.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "types.c"
#include <time.h>

int game_is_running = 0;
SDL_Surface *backbuffer = NULL;

SDL_Rect drag_rect;
RGB drag_rect_color;

Gui_Object button;
SDL_Rect button_rect;

SDL_Texture *backbufferTexture = NULL;

bool is_selecting = false;

int last_frame_time = 0;

void convert_hex_to_rgb(const char *input, RGB *input_shape)
{
    if (input[0] == '#')
    {
        input++;
    }
    sscanf(input, "%02x%02x%02x", &input_shape->r, &input_shape->g, &input_shape->b);
}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int initialize_window()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        fprintf(stderr, "ERROR Initializing SDL.\n");
        return FALSE;
    }

    window = SDL_CreateWindow(NULL,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              SDL_WINDOW_RESIZABLE);
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

// Function to create the backbuffer with the same dimensions as the window
void create_backbuffer(SDL_Renderer *renderer, int window_width, int window_height)
{
    if (backbuffer != NULL)
    {
        SDL_FreeSurface(backbuffer);
    }

    backbuffer = SDL_CreateRGBSurface(0, window_width, window_height, 32, 0, 0, 0, 0);

    if (backbuffer == NULL)
    {
        fprintf(stderr, "Error creating the backbuffer %s\n", SDL_GetError());
    }
}

void resize_window_retain_aspect(int newWidth, int newHeight)
{
    float originalAspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

    int adjustedWidth = newWidth;
    int adjustedHeight = (int)(adjustedWidth / originalAspectRatio);
    create_backbuffer(renderer, adjustedWidth, adjustedHeight);
    SDL_SetWindowSize(window, adjustedWidth, adjustedHeight);
    SDL_RenderSetLogicalSize(renderer, adjustedWidth, adjustedHeight);
}

void setup()
{
    convert_hex_to_rgb(DRAG_RECT_HEX, &drag_rect_color);

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
            if ((event.button.x >= button_rect.x && event.button.x <= (button_rect.x + button_rect.w)) &&
                (event.button.y >= button_rect.y && event.button.y <= (button_rect.y + button_rect.h)))
            {
                button.clicked = true;
            }
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
            button.clicked = false;
            drag_rect.h = 0;
            drag_rect.w = 0;
        }
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            int newWidth = event.window.data1;
            int newHeight = event.window.data2;
            resize_window_retain_aspect(newWidth, newHeight);
        }
        break;
    }
}

void render_with_backbuffer(SDL_Renderer *renderer, int window_width, int window_height)
{
    RGB background_color;
    convert_hex_to_rgb("#cbd5e1", &background_color);
    // Set the draw color for the backbuffer
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, 255);

    // Clear the backbuffer
    SDL_RenderClear(renderer);

    // Set the draw color for the green square
    SDL_SetRenderDrawColor(renderer, drag_rect_color.r, drag_rect_color.g, drag_rect_color.b, 255);

    // Render onto the backbuffer
    if (is_selecting)
    {
        SDL_RenderFillRect(renderer, &drag_rect);
    }
    if (button.clicked)
    {
        const char *lighter_indigo = "#4f46e5";
        convert_hex_to_rgb(lighter_indigo, &button.bg);
    } else {
       const char *indigo = "#2e1065";
        convert_hex_to_rgb(indigo, &button.bg); 
    }

    int width = 0;
    int height = 0;

    SDL_GetWindowSize(window, &width, &height);

    button_rect.w = width / 3;
    button_rect.h = height;
    button_rect.x = width - (button_rect.w);
    button_rect.y = height - (button_rect.h);

    SDL_SetRenderDrawColor(renderer, button.bg.r, button.bg.g, button.bg.b, 255);
    SDL_RenderFillRect(renderer, &button_rect);

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

void update()
{
    // float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

    // last_frame_time = SDL_GetTicks();
}

void destroy_app()
{
    if (backbufferTexture)
    {
        SDL_DestroyTexture(backbufferTexture);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    game_is_running = initialize_window();

    create_backbuffer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    setup();

    while (game_is_running)
    {
        process_input();
        update();
        render_with_backbuffer(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    destroy_app();
    return 0;
}