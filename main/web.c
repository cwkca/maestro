/** Emscripten entry point and event loop. */

#include <SDL.h>
#include <emscripten.h>

#include "app.h"

void main_loop()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            emscripten_cancel_main_loop();
        else
            handle_event(event);
    }
}

int main(int argc, char **argv)
{
    if (app_init() || app_start())
        return 1;

    // 0 fps means use requestAnimationFrame (recommended)
    emscripten_set_main_loop(main_loop, 0, 1);

    app_cleanup();
    return 0;
}
