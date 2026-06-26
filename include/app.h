#ifndef APP_H
#define APP_H

#include <SDL.h>

int app_init();
int app_start();
void handle_event(SDL_Event event);
void app_cleanup();

#endif /* APP_H */
