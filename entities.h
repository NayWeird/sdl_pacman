#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    SDL_Rect rect;
    bool pursuing_plr;
    bool dead;
    int time_dead;
    int direction;

} Ghost;

typedef struct {
    SDL_Rect rect;
    bool moving;
    int x_speed;
    int y_speed;
    int direction;
    int intention;
    int lives;
} Player;

bool collided(int *map, int w, SDL_Rect *plr, SDL_Rect *ghost);
int get_blinkys_direction(int *map, int w, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction);
int get_pinkys_direction(int *map, int w, int x_speed, int y_speed, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction);
int get_inkys_direction(int *map, int w, int x_speed, int y_speed, SDL_Rect *plr, SDL_Rect *blinky, SDL_Rect *ghost_rect, int current_direction);
int get_clydes_direction(int *map, int w,SDL_Point *alt_target, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction);

void portal_travel(int w, SDL_Rect *plr, SDL_Rect *r, SDL_Rect *p, SDL_Rect *c, SDL_Rect *o);