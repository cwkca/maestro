#ifndef APP_H
#define APP_H

#include <SDL.h>

int app_init();
int app_start();
void handle_key(SDL_Keysym keysym);
void app_cleanup();

#endif /* APP_H */
