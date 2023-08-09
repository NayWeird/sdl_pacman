#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sdl.h"
#include "entities.h"

int main()
{
    
    const int t_size = 16;
    const int e_size = t_size * 2;
    int w = 0;
    int h = 0;
    char buffer[255];

    bool boosted = false;
    bool point_picked = false;
    int total_points = 0;
    int score = 0;
    int ghost_streak = 0;

    bool respawning = true;
    int respawn_time = 0;
    int t = 0;
    int t_counter = 0;
    int a_speed = 0;
    int boost_duration = 0;
    int white_ghost = 0;
    
    FILE * file;
    if(!(file = fopen("map.txt", "r"))){
        return 0;
    }

    srand(time(NULL));

    fgets(buffer, sizeof(buffer), file);    // Reads the first row containing the file map dimensions
 
    w = atoi(strtok(buffer, " "));
    h = atoi(strtok(NULL, " "));
 
    int *map = malloc(sizeof(int) * w * h);

    if (SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("SDL experiments", 100, 100, t_size * w, t_size * h + 50, SDL_WINDOW_SHOWN); // 240 * 256
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if(TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Font
    TTF_Font *arial = TTF_OpenFont("Arial.ttf", 16);
    SDL_Rect score_rect = { .x = 0, .y = 522, .w = 16, .h = 32};
    SDL_Color white = {255, 255, 255, 255};
    char score_str[6] = "00";

    // Asset map
    SDL_Surface *surf = IMG_Load("pacman_asset_map.png");
    SDL_Texture *assets = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect src_rect = {.x = 0, .y = 0, .w = t_size, .h = t_size};

    SDL_Rect plr = { .x = 0, .y = 0, .w = e_size, .h = e_size};
    
    SDL_Rect blinky_random = { .x = 0, .y = 0, .w = 0, .h = 0};
    SDL_Rect pinky_random = { .x = 0, .y = 0, .w = 0, .h = 0};
    SDL_Rect inky_random = { .x = 0, .y = 0, .w = 0, .h = 0};
    SDL_Rect clyde_random = { .x = 0, .y = 0, .w = 0, .h = 0};

    SDL_Rect blinky_spawn = { .x = 0, .y = 0};
    SDL_Rect inky_spawn = { .x = 0, .y = 0};
    SDL_Rect pinky_spawn = { .x = 0, .y = 0};
    SDL_Rect clyde_spawn = { .x = 0, .y = 0};
    
    SDL_Point clydes_target = { .x = 2, .y = h};

    SDL_Rect l = { .x = w * t_size / 2, .y = 522, .w = e_size, .h = e_size };

    
    // 0 - Air, 1 - Wall, 2 - Gate, 3 - Point, 4 - Booster
    // 5 - Player, 6 - Blinky_rect, 7 - Pinky, 8 - Inky, 9 - clyde_rect
 
    // Writes contents of text file into easier to read int array
    for(int i = 0 ; i < h; i ++){
        fgets(buffer, sizeof(buffer), file);
        for(int j = 0; j < w; j ++){
            switch (buffer[j])
            {
            case ' ':   // Air
                map[i * w + j] = 0;
                break;
            case 'x':   // Wall
                map[i * w + j] = 1;
                break;
            case 'd':   // Door
                map[i * w + j] = 2;
                break;
            case '.':   // Point
                map[i * w + j] = 3;
                total_points++;
                break;
            case 'y':   // Player
                map[i * w + j] = 5;
                plr.x = j;
                plr.y = i;
                break;
            case 'r':   // Blinky_rect
                map[i * w + j] = 6;
                blinky_spawn.x = j;
                blinky_spawn.y = i;
                break;
            case 'c':   // Inky
                map[i * w + j] = 7;
                inky_spawn.x = j;
                inky_spawn.y = i;
                break;
            case 'p':   // Pinky
                map[i * w + j] = 8;
                pinky_spawn.x = j;
                pinky_spawn.y = i;
                break;
            case 'o':   // clyde_rect
                map[i * w + j] = 9;
                clyde_spawn.x = j;
                clyde_spawn.y = i;
                break;
            default:
                break;
            }
        }
    }

    Player pacman = { .rect = plr, .moving = false, .x_speed = 0, .y_speed = 0, .direction = 0, .intention = 0, .lives = 2};
    Ghost blinky = { .rect = blinky_spawn, .pursuing_plr = true, .dead = false, .time_dead = 0, .direction = 1};
    Ghost inky = { .rect = inky_spawn, .pursuing_plr = true, .dead = false, .time_dead = 0, .direction = 2};
    Ghost pinky = { .rect = pinky_spawn, .pursuing_plr = true, .dead = false, .time_dead = 0, .direction = 3};
    Ghost clyde = { .rect = clyde_spawn, .pursuing_plr = true, .dead = false, .time_dead = 0, .direction = 1};

    // Generates 4 boosters on the map randomly
    int boost_count = 0;
    while(boost_count < 4) {
        for(int i = 0; i < h; i++) {
            if(boost_count >= 4) {
                break;
            }
            for(int j = 0; j < w; j++) {
                if(map[i * w + j] == 3) {
                    if(boost_count >= 4) {
                        break;
                    }
                    if(rand() % 200 == 1) {
                        map[i * w + j] = 4;
                        boost_count++;
                    }
                }
            }
        }
    }

    SDL_Event e;
    bool quit = false;

    while (!quit && pacman.lives >= 0 && total_points > 0)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type == SDL_KEYDOWN && !respawning) 
            {
                switch(e.key.keysym.sym) {
                    case SDLK_a:
                        pacman.x_speed = -1;
                        pacman.direction = 2;
                        if(!pacman.moving) {
                            pacman.intention = 0;
                            pacman.moving = true;
                        }
                        else {
                            pacman.intention = 2;
                        }
                        break;
                    case SDLK_d:
                        pacman.x_speed = 1;
                        pacman.direction = 1;
                        if(!pacman.moving) {
                            pacman.intention = 0;
                            pacman.moving = true;
                        }
                        else {
                            pacman.intention = 1;
                        }
                        break;
                    case SDLK_w:
                        pacman.y_speed = -1;
                        pacman.direction = 3;
                        if(!pacman.moving) {
                            pacman.intention = 0;
                            pacman.moving = true;
                        }
                        else {
                            pacman.intention = 3;
                        }
                        break;
                    case SDLK_s:
                        pacman.y_speed = 1;
                        pacman.direction = 4;
                        if(!pacman.moving) {
                            pacman.intention = 0;
                            pacman.moving = true;
                        }
                        else {
                            pacman.intention = 4;
                        }
                        break;
                    default:
                        pacman.x_speed = 0;
                        pacman.y_speed = 0;
                        pacman.moving = false;
                }
            }
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Nastavení barvy na černou
        SDL_RenderClear(renderer);                      // Renders the background

        // Writes score on the screen
        if(score < 10) {
            score_rect.w = t_size;
        }
        else if(score < 100) {
            score_rect.w = 2 * t_size;
        }
        else if(score < 1000) {
            score_rect.w = 3 * t_size;
        }
        else if(score < 10000) {
            score_rect.w = 4 * t_size;
        }
        else {
            score_rect.w = 5 * t_size;
        }
        sdl_draw_text(renderer, arial, white, score_rect, score_str);
        
        // Displays lives
        if(pacman.lives > 1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            l.x = (w * t_size / 2) + (2 * e_size);
            SDL_RenderFillRect(renderer, &l);
        }
        if(pacman.lives > 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            l.x = (w * t_size / 2) + (4 * e_size);
            SDL_RenderFillRect(renderer, &l);
        }

        if(respawning) {
            respawn_time++;
            pacman.rect.x = 14;
            pacman.rect.y = 23;
            pacman.direction = 0;
            blinky.direction = 1;
            inky.direction = 2;
            pinky.direction = 3;
            clyde.direction = 1;
            if(respawn_time > 100) {
                respawning = false;
                respawn_time = 0;
            }
        }

        // Renders the entire scene
        for(int i = 0; i < h; i++) {  
            for(int j = 0; j < w; j++) {
                int block = map[(i * w) + j];
                switch(block) {
                    case 0: // Air
                        break;
                    case 1: // Wall
                        SDL_SetRenderDrawColor(renderer, 16, 0, 192, 255);
                        SDL_Rect w = { .x = j * t_size, .y = i * t_size, .w = t_size, .h = t_size};
                        SDL_RenderFillRect(renderer, &w);
                        break;
                    case 2: // Gate
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_Rect d = { .x = j * t_size, .y = i * t_size + 4, .w = t_size, .h = t_size / 2};
                        SDL_RenderFillRect(renderer, &d);
                        break;
                    case 3: // Point
                        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                        SDL_RenderDrawPoint(renderer, j * t_size, i * t_size);
                        break;
                    case 4: // Booster
                        SDL_SetRenderDrawColor(renderer, 255, 255, 32, 255);
                        SDL_Rect xp = { .x = j * t_size - 4, .y = i * t_size - 4, .w = t_size / 2, .h = t_size / 2 };
                        SDL_RenderFillRect(renderer, &xp);
                        break;
                    case 5: // Player
                        SDL_Rect p = { .x = pacman.rect.x * t_size, .y = pacman.rect.y * t_size, .w = e_size, .h = e_size};
                        if(pacman.direction == 0) {
                            src_rect.x = 33;
                            src_rect.y = 0;
                        }
                        else {
                            if(point_picked) {
                                src_rect.x = 17;
                                src_rect.y = (pacman.direction - 1) * t_size;
                            }
                            else {
                                src_rect.x = 1;
                                src_rect.y = (pacman.direction - 1) * t_size;
                            }
                        }
                        SDL_RenderCopy(renderer, assets, &src_rect, &p);
                        break;
                    case 6: // Blinky
                        SDL_Rect red = { .x = blinky.rect.x * t_size, .y = blinky.rect.y * t_size, .w = e_size, .h = e_size };
                        if(blinky.dead) {   // Eyes only
                            src_rect.x = 2 + ((blinky.direction + 7) * t_size);
                            src_rect.y = 5 * t_size;
                        }
                        else if(boosted && !blinky.pursuing_plr) {
                            if(boost_duration > 35 && white_ghost < 6) {    // White animation
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (11 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (10 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                            else {  // Blue animation
                                if(white_ghost > 12) {
                                    white_ghost = 0;
                                }
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (9 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (8 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                        }
                        else {  // Regular animation
                            if(a_speed % 2 == 0) {
                                src_rect.x = 2 + (((blinky.direction * 2) - 1) * t_size);
                                src_rect.y = 4 * t_size;
                            }
                            else {
                                src_rect.x = 2 + (((blinky.direction - 1) * 2) * t_size);
                                src_rect.y = 4 * t_size;
                            }
                        }
                        SDL_RenderCopy(renderer, assets, &src_rect, &red);
                        break;
                    case 7: // Pinky
                        SDL_Rect pink = { .x = pinky.rect.x * t_size, .y = pinky.rect.y * t_size, .w = e_size, .h = e_size };
                        if(pinky.dead) {   // Eyes only
                            src_rect.x = 2 + ((pinky.direction + 7) * t_size);
                            src_rect.y = 5 * t_size;
                        }
                        else if(boosted && !pinky.pursuing_plr) {
                            if(boost_duration > 35 && white_ghost < 6) {    // White animation
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (11 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (10 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                            else {  // Blue animation
                                if(white_ghost > 12) {
                                    white_ghost = 0;
                                }
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (9 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (8 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                        }
                        else {  // Regular animation
                            if(a_speed % 2 == 0) {
                                src_rect.x = 2 + (((pinky.direction * 2) - 1) * t_size);
                                src_rect.y = 5 * t_size;
                            }
                            else {
                                src_rect.x = 2 + (((pinky.direction - 1) * 2) * t_size);
                                src_rect.y = 5 * t_size;
                            }
                        }
                        SDL_RenderCopy(renderer, assets, &src_rect, &pink);
                        break;
                    case 8: // Inky
                        SDL_Rect cyan = { .x = inky.rect.x * t_size, .y = inky.rect.y * t_size, .w = e_size, .h = e_size };
                        if(inky.dead) {   // Eyes only
                            src_rect.x = 2 + ((inky.direction + 7) * t_size);
                            src_rect.y = 5 * t_size;
                        }
                        else if(boosted && !inky.pursuing_plr) {
                            if(boost_duration > 35 && white_ghost < 6) {    // White animation
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (11 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (10 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                            else {  // Blue animation
                                if(white_ghost > 12) {
                                    white_ghost = 0;
                                }
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (9 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (8 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                        }
                        else {  // Regular animation
                            if(a_speed % 2 == 0) {
                                src_rect.x = 2 + (((inky.direction * 2) - 1) * t_size);
                                src_rect.y = 6 * t_size;
                            }
                            else {
                                src_rect.x = 2 + (((inky.direction - 1) * 2) * t_size);
                                src_rect.y = 6 * t_size;
                            }
                        }
                        SDL_RenderCopy(renderer, assets, &src_rect, &cyan);
                        break;
                    case 9: // Clyde
                        SDL_Rect orange = { .x = clyde.rect.x * t_size, .y = clyde.rect.y * t_size, .w = e_size, .h = e_size };
                        if(clyde.dead) {   // Eyes only
                            src_rect.x = 2 + ((clyde.direction + 7) * t_size);
                            src_rect.y = 5 * t_size;
                        }
                        else if(boosted && !clyde.pursuing_plr) {
                            if(boost_duration > 35 && white_ghost < 6) {    // White animation
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (11 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (10 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                            else {  // Blue animation
                                if(white_ghost > 12) {
                                    white_ghost = 0;
                                }
                                white_ghost++;
                                if(a_speed % 2 == 0) {
                                    src_rect.x = 2 + (9 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                                else {
                                    src_rect.x = 2 + (8 * t_size);
                                    src_rect.y = 4 * t_size;
                                }
                            }
                        }
                        else {  // Regular animation
                            if(a_speed % 2 == 0) {
                                src_rect.x = 2 + (((clyde.direction * 2) - 1) * t_size);
                                src_rect.y = 7 * t_size;
                            }
                            else {
                                src_rect.x = 2 + (((clyde.direction - 1) * 2) * t_size);
                                src_rect.y = 7 * t_size;
                            }
                        }
                        SDL_RenderCopy(renderer, assets, &src_rect, &orange);
                        break;
                    default:
                        break;
                }
            }
        }

        SDL_RenderPresent(renderer);  // Prezentace kreslítka

        if(t < 7) {
            if(t > 2) {
                point_picked = false;
            }
            t++;
            if(t % 4 == 0) {
                a_speed++;
            }
            if(a_speed == 6) {
                a_speed = 0;
            }
        }

        if(t >= 7) {
            t = 0;
            if(!respawning) {
                t_counter++;
            }
            else {
                t_counter = 0;
            }

            // Checking for ghost state
            if(boosted) {
                // Scared Blinky
                if(!blinky.dead) {
                    blinky.direction = get_blinkys_direction(map, w, &blinky_random, &blinky.rect, blinky.direction);
                }
                if(collided(map, w, &pacman.rect, &blinky.rect)) {
                    blinky.time_dead++;
                    if(!blinky.dead) {
                        ghost_streak++;
                        score += ghost_streak * 200;
                    }
                    blinky.dead = true;
                }
                // Scared Pinky
                if(!pinky.dead) {
                    pinky.direction = get_blinkys_direction(map, w, &pinky_random, &pinky.rect, pinky.direction);
                }
                if(collided(map, w, &pacman.rect, &pinky.rect)) {
                    pinky.time_dead++;
                    if(!pinky.dead) {
                        ghost_streak++;
                        score += ghost_streak * 200;
                    }
                    pinky.dead = true;
                }
                // Scared Inky
                if(!inky.dead) {
                    inky.direction = get_blinkys_direction(map, w, &inky_random, &inky.rect, inky.direction);
                }
                if(collided(map, w, &pacman.rect, &inky.rect)) {
                    inky.time_dead++;
                    if(!inky.dead) {
                        ghost_streak++;
                        score += ghost_streak * 200;
                    }
                    inky.dead = true;
                }
                // Scared Clyde
                if(!clyde.dead) {
                    clyde.direction = get_blinkys_direction(map, w, &clyde_random, &clyde.rect, clyde.direction);
                }
                if(collided(map, w, &pacman.rect, &clyde.rect)) {
                    clyde.time_dead++;
                    if(!clyde.dead) {
                        ghost_streak++;
                        score += ghost_streak * 200;
                    }
                    clyde.dead = true;
                }
            }

            // Blinky
            if(blinky.dead && blinky.time_dead > 0) {
                blinky.time_dead++;
                blinky.direction = get_blinkys_direction(map, w, &blinky_spawn, &blinky.rect, blinky.direction);
                if(blinky.time_dead > 80) {
                    blinky.rect.x = blinky_spawn.x;
                    blinky.rect.y = blinky_spawn.y;
                    blinky.direction = 1;
                    blinky.pursuing_plr = true;
                }
                if(blinky.rect.x == blinky_spawn.x && blinky.rect.y == blinky_spawn.y) {
                    blinky.dead = false;
                    blinky.time_dead = 0;
                    blinky.pursuing_plr = true;
                }
            }
            else {
                blinky.direction = get_blinkys_direction(map, w, &pacman.rect, &blinky.rect, blinky.direction);
            }

            // Pinky
            if(pinky.dead && pinky.time_dead > 0) {
                pinky.time_dead++;
                pinky.direction = get_blinkys_direction(map, w, &blinky_spawn, &pinky.rect, pinky.direction);
                if(pinky.time_dead > 80) {
                    pinky.rect.x = blinky_spawn.x;
                    pinky.rect.y = blinky_spawn.y;
                    pinky.direction = 1;
                    pinky.pursuing_plr = true;
                }
                if(pinky.rect.x == blinky_spawn.x && pinky.rect.y == blinky_spawn.y) {
                    pinky.dead = false;
                    pinky.time_dead = 0;
                    pinky.pursuing_plr = true;
                }
            }
            else {
                pinky.direction = get_pinkys_direction(map, w, pacman.x_speed, pacman.y_speed, &pacman.rect, &pinky.rect, pinky.direction);
            }

            // Inky
            if(inky.dead && inky.time_dead > 0) {
                inky.time_dead++;
                inky.direction = get_blinkys_direction(map, w, &blinky_spawn, &inky.rect, inky.direction);
                if(inky.time_dead > 80) {
                    inky.rect.x = blinky_spawn.x;
                    inky.rect.y = blinky_spawn.y;
                    inky.direction = 1;
                    inky.pursuing_plr = true;
                }
                if(inky.rect.x == blinky_spawn.x && inky.rect.y == blinky_spawn.y) {
                    inky.dead = false;
                    inky.time_dead = 0;
                    inky.pursuing_plr = true;
                }
            }
            else {
                inky.direction = get_inkys_direction(map, w, pacman.x_speed, pacman.y_speed, &pacman.rect, &blinky.rect, &inky.rect, inky.direction);
            }
            
            // Clyde
            if(clyde.dead && clyde.time_dead > 0) {
                clyde.time_dead++;
                clyde.direction = get_blinkys_direction(map, w, &blinky_spawn, &clyde.rect, clyde.direction);
                if(clyde.time_dead > 80) {
                    clyde.rect.x = blinky_spawn.x;
                    clyde.rect.y = blinky_spawn.y;
                    clyde.direction = 1;
                    clyde.pursuing_plr = true;
                }
                if(clyde.rect.x == blinky_spawn.x && clyde.rect.y == blinky_spawn.y) {
                    clyde.dead = false;
                    clyde.time_dead = 0;
                    clyde.pursuing_plr = true;
                }
            }
            else {
                clyde.direction = get_clydes_direction(map, w, &clydes_target, &pacman.rect, &clyde.rect, clyde.direction);
            }

            if(!boosted && ((!blinky.dead && collided(map, w, &pacman.rect, &blinky.rect)) || (!pinky.dead && collided(map, w, &pacman.rect, &pinky.rect)) || (!inky.dead && collided(map, w, &pacman.rect, &inky.rect)) || (!clyde.dead && collided(map, w, &pacman.rect, &clyde.rect)))) {
                pacman.lives--;
                    blinky.rect.x = blinky_spawn.x;
                    blinky.rect.y = blinky_spawn.y;
                    inky.rect.x = inky_spawn.x;
                    inky.rect.y = inky_spawn.y;
                    pinky.rect.x = pinky_spawn.x;
                    pinky.rect.y = pinky_spawn.y;
                    clyde.rect.x = clyde_spawn.x;
                    clyde.rect.y = clyde_spawn.y;
                    respawning = true;
            }

            // Ghost movement based on direction
            if(!respawning) {
                switch(blinky.direction) {  // Blinky
                    case 1:
                        blinky.rect.x += 1;
                        blinky.rect.y += 0;
                        break;
                    case 2:
                        blinky.rect.x += -1;
                        blinky.rect.y += 0;
                        break;
                    case 3:
                        blinky.rect.x += 0;
                        blinky.rect.y += -1;
                        break;
                    case 4:
                        blinky.rect.x += 0;
                        blinky.rect.y += 1;
                        break;
                    default:
                        if(map[((blinky.rect.y - 1) * w) + blinky.rect.x] != 1 && map[((blinky.rect.y - 1) * w) + blinky.rect.x + 1] != 1) {// Up
                            blinky.rect.x += 0;
                            blinky.rect.y -= 1;
                            blinky.direction = 3;
                            break;
                        }
                        if(map[(blinky.rect.y * w) + blinky.rect.x - 1] != 1 && map[((blinky.rect.y + 1) * w) + blinky.rect.x - 1] != 1) {  // Left
                            blinky.rect.x -= 1;
                            blinky.rect.y += 0;
                            blinky.direction = 2;
                            break;
                        }
                        if(map[((blinky.rect.y + 2) * w) + blinky.rect.x] != 1 && map[((blinky.rect.y + 2) * w) + blinky.rect.x + 1] != 1) {// Down
                            blinky.rect.x += 0;
                            blinky.rect.y += 1;
                            blinky.direction = 4;
                            break;
                        }
                        if(map[(blinky.rect.y * w) + blinky.rect.x + 2] != 1 && map[((blinky.rect.y + 1) * w) + blinky.rect.x + 2] != 1) {  // Right
                            blinky.rect.x += 1;
                            blinky.rect.y += 0;
                            blinky.direction = 1;
                            break;
                        }
                        blinky.direction = 0;
                        break;
                }

                // Ghosts are released after a certain amount of time
                if(t_counter == 12) {
                    pinky.rect.x = blinky_spawn.x;
                    pinky.rect.y = blinky_spawn.y;
                }
                if(t_counter == 24) {
                    inky.rect.x = blinky_spawn.x;
                    inky.rect.y = blinky_spawn.y;
                }
                if(t_counter == 36) {
                    clyde.rect.x = blinky_spawn.x;
                    clyde.rect.y = blinky_spawn.y;
                }

                if(t_counter > 12) {    // Pinky
                    switch(pinky.direction) {
                        case 1:
                            pinky.rect.x += 1;
                            pinky.rect.y += 0;
                            break;
                        case 2:
                            pinky.rect.x += -1;
                            pinky.rect.y += 0;
                            break;
                        case 3:
                            pinky.rect.x += 0;
                            pinky.rect.y += -1;
                            break;
                        case 4:
                            pinky.rect.x += 0;
                            pinky.rect.y += 1;
                            break;
                        default:
                            if(map[((pinky.rect.y - 1) * w) + pinky.rect.x] != 1 && map[((pinky.rect.y - 1) * w) + pinky.rect.x + 1] != 1) {// Up
                                pinky.rect.x += 0;
                                pinky.rect.y -= 1;
                                pinky.direction = 3;
                                break;
                            }
                            if(map[(pinky.rect.y * w) + pinky.rect.x - 1] != 1 && map[((pinky.rect.y + 1) * w) + pinky.rect.x - 1] != 1) {  // Left
                                pinky.rect.x -= 1;
                                pinky.rect.y += 0;
                                pinky.direction = 2;
                                break;
                            }
                            if(map[((pinky.rect.y + 2) * w) + pinky.rect.x] != 1 && map[((pinky.rect.y + 2) * w) + pinky.rect.x + 1] != 1) {// Down
                                pinky.rect.x += 0;
                                pinky.rect.y += 1;
                                pinky.direction = 4;
                                break;
                            }
                            if(map[(pinky.rect.y * w) + pinky.rect.x + 2] != 1 && map[((pinky.rect.y + 1) * w) + pinky.rect.x + 2] != 1) {  // Right
                                pinky.rect.x += 1;
                                pinky.rect.y += 0;
                                pinky.direction = 1;
                                break;
                            }
                            pinky.direction = 0;
                            break;
                    }
                }
                
                if(t_counter > 24) {    // Inky
                    switch(inky.direction) {
                        case 1:
                            inky.rect.x += 1;
                            inky.rect.y += 0;
                            break;
                        case 2:
                            inky.rect.x += -1;
                            inky.rect.y += 0;
                            break;
                        case 3:
                            inky.rect.x += 0;
                            inky.rect.y += -1;
                            break;
                        case 4:
                            inky.rect.x += 0;
                            inky.rect.y += 1;
                            break;
                        default:
                            if(map[((inky.rect.y - 1) * w) + inky.rect.x] != 1 && map[((inky.rect.y - 1) * w) + inky.rect.x + 1] != 1) {// Up
                                inky.rect.x += 0;
                                inky.rect.y -= 1;
                                inky.direction = 3;
                                break;
                            }
                            if(map[(inky.rect.y * w) + inky.rect.x - 1] != 1 && map[((inky.rect.y + 1) * w) + inky.rect.x - 1] != 1) {  // Left
                                inky.rect.x -= 1;
                                inky.rect.y += 0;
                                inky.direction = 2;
                                break;
                            }
                            if(map[((inky.rect.y + 2) * w) + inky.rect.x] != 1 && map[((inky.rect.y + 2) * w) + inky.rect.x + 1] != 1) {// Down
                                inky.rect.x += 0;
                                inky.rect.y += 1;
                                inky.direction = 4;
                                break;
                            }
                            if(map[(inky.rect.y * w) + inky.rect.x + 2] != 1 && map[((inky.rect.y + 1) * w) + inky.rect.x + 2] != 1) {  // Right
                                inky.rect.x += 1;
                                inky.rect.y += 0;
                                inky.direction = 1;
                                break;
                            }
                            inky.direction = 0;
                            break;
                    }
                }

                if(t_counter > 36) {    // Clyde
                    switch(clyde.direction) {
                        case 1:
                            clyde.rect.x += 1;
                            clyde.rect.y += 0;
                            break;
                        case 2:
                            clyde.rect.x += -1;
                            clyde.rect.y += 0;
                            break;
                        case 3:
                            clyde.rect.x += 0;
                            clyde.rect.y += -1;
                            break;
                        case 4:
                            clyde.rect.x += 0;
                            clyde.rect.y += 1;
                            break;
                        default:
                            if(map[((clyde.rect.y - 1) * w) + clyde.rect.x] != 1 && map[((clyde.rect.y - 1) * w) + clyde.rect.x + 1] != 1) {// Up
                                clyde.rect.x += 0;
                                clyde.rect.y -= 1;
                                clyde.direction = 3;
                                break;
                            }
                            if(map[(clyde.rect.y * w) + clyde.rect.x - 1] != 1 && map[((clyde.rect.y + 1) * w) + clyde.rect.x - 1] != 1) {  // Left
                                clyde.rect.x -= 1;
                                clyde.rect.y += 0;
                                clyde.direction = 2;
                                break;
                            }
                            if(map[((clyde.rect.y + 2) * w) + clyde.rect.x] != 1 && map[((clyde.rect.y + 2) * w) + clyde.rect.x + 1] != 1) {// Down
                                clyde.rect.x += 0;
                                clyde.rect.y += 1;
                                clyde.direction = 4;
                                break;
                            }
                            if(map[(clyde.rect.y * w) + clyde.rect.x + 2] != 1 && map[((clyde.rect.y + 1) * w) + clyde.rect.x + 2] != 1) {  // Right
                                clyde.rect.x += 1;
                                clyde.rect.y += 0;
                                clyde.direction = 1;
                                break;
                            }
                            clyde.direction = 0;
                            break;
                    }
                }
            }

            if(boosted) {
                if(boost_duration % 15 == 0) {  // Moving randomly while pacman is boosted
                    blinky_random.x = (rand() % w) + 1;
                    blinky_random.y = (rand() % h) + 1;
                    inky_random.x = (rand() % w) + 1;
                    inky_random.y = (rand() % h) + 1;
                    pinky_random.x = (rand() % w) + 1;
                    pinky_random.y = (rand() % h) + 1;
                    clyde_random.x = (rand() % w) + 1;
                    clyde_random.y = (rand() % h) + 1;
                }
                if(boost_duration > 50) {   // disabling boost
                    boost_duration = 0;
                    ghost_streak = 0;
                    white_ghost = 0;
                    boosted = false;
                    blinky.pursuing_plr = true;
                    pinky.pursuing_plr = true;
                    inky.pursuing_plr = true;
                    clyde.pursuing_plr = true;
                }
                boost_duration++;
            }
            
            portal_travel(w, &pacman.rect, &blinky.rect, &pinky.rect, &inky.rect, &clyde.rect);
            
            // Player turns direction if possible
            if(pacman.intention > 0) {
                switch(pacman.intention) {
                    case 1: // turning right
                        if((map[(pacman.rect.y * w) + pacman.rect.x + 2] == 0 || map[(pacman.rect.y * w) + pacman.rect.x + 2] == 3 || map[(pacman.rect.y * w) + pacman.rect.x + 2] == 4) && (map[((pacman.rect.y + 1) * w) + pacman.rect.x + 2] == 0 || map[((pacman.rect.y + 1) * w) + pacman.rect.x + 2] == 3 || map[((pacman.rect.y + 1) * w) + pacman.rect.x + 2] == 4)) {
                            pacman.x_speed = 1;
                            pacman.y_speed = 0;
                        }
                        break;
                    case 2: // turning left
                        if((map[(pacman.rect.y * w) + pacman.rect.x - 1] == 0 || map[(pacman.rect.y * w) + pacman.rect.x - 1] == 3 || map[(pacman.rect.y * w) + pacman.rect.x - 1] == 4) && (map[((pacman.rect.y + 1) * w) + pacman.rect.x - 1] == 0 || map[((pacman.rect.y + 1) * w) + pacman.rect.x - 1] == 3 || map[((pacman.rect.y + 1) * w) + pacman.rect.x - 1] == 4)) {
                            pacman.x_speed = -1;
                            pacman.y_speed = 0;
                        }
                        break;
                    case 3: // turning up
                        if((map[((pacman.rect.y - 1) * w) + pacman.rect.x] == 0 || map[((pacman.rect.y - 1) * w) + pacman.rect.x] == 3 || map[((pacman.rect.y - 1) * w) + pacman.rect.x] == 4) && (map[((pacman.rect.y - 1) * w) + pacman.rect.x + 1] == 0 || map[((pacman.rect.y - 1) * w) + pacman.rect.x + 1] == 3 || map[((pacman.rect.y - 1) * w) + pacman.rect.x + 1] == 4)) {
                            pacman.x_speed = 0;
                            pacman.y_speed = -1;
                        }
                        break;
                    case 4: // turning down
                        if((map[((pacman.rect.y + 2) * w) + pacman.rect.x] == 0 || map[((pacman.rect.y + 2) * w) + pacman.rect.x] == 3 || map[((pacman.rect.y + 2) * w) + pacman.rect.x] == 4) && (map[((pacman.rect.y + 2) * w) + pacman.rect.x + 1] == 0 || map[((pacman.rect.y + 2) * w) + pacman.rect.x + 1] == 3 || map[((pacman.rect.y + 2) * w) + pacman.rect.x + 1] == 4)) {
                            pacman.x_speed = 0;
                            pacman.y_speed = 1;
                        }
                        break;
                }
            }

            if(pacman.x_speed == 1) {  // Going right
                if(map[((pacman.rect.y  + 1) * w) + pacman.rect.x + 2] == 1 || map[(pacman.rect.y * w) + pacman.rect.x + 2] == 1 || map[((pacman.rect.y  + 1) * w) + pacman.rect.x + 2] == 2 || map[(pacman.rect.y * w) + pacman.rect.x + 2] == 2) {
                    pacman.x_speed = 0;
                    pacman.moving = false;
                }
                else {
                    pacman.direction = 1;
                }
                
            }
            if(pacman.x_speed == -1) { // Going left
                if(map[(pacman.rect.y * w) + pacman.rect.x - 1] == 1 || map[((pacman.rect.y + 1) * w) + pacman.rect.x - 1] == 1 || map[(pacman.rect.y * w) + pacman.rect.x - 1] == 2 || map[((pacman.rect.y + 1) * w) + pacman.rect.x - 1] == 2) {
                    pacman.x_speed = 0;
                    pacman.moving = false;
                }
                else {
                    pacman.direction = 2;
                }
            }
            if(pacman.y_speed == 1) {  // Going down
                if(map[((pacman.rect.y + 2) * w) + pacman.rect.x] == 1 || map[((pacman.rect.y + 2) * w) + pacman.rect.x + 1] == 1 || map[((pacman.rect.y + 2) * w) + pacman.rect.x] == 2 || map[((pacman.rect.y + 2) * w) + pacman.rect.x + 1] == 2) {
                    pacman.y_speed = 0;
                    pacman.moving = false;
                }
                else {
                    pacman.direction = 4;
                }
            }
            if(pacman.y_speed == -1) { // Going up
                if(map[((pacman.rect.y  - 1) * w) + pacman.rect.x] == 1 || map[((pacman.rect.y - 1) * w) + pacman.rect.x + 1] == 1 || map[((pacman.rect.y  - 1) * w) + pacman.rect.x] == 2 || map[((pacman.rect.y - 1) * w) + pacman.rect.x + 1] == 2) {
                    pacman.y_speed = 0;
                    pacman.moving = false;
                }
                else {
                    pacman.direction = 3;
                }
            }
            if(pacman.x_speed != 0 || pacman.y_speed != 0) {
                pacman.moving = true;
            }

            // Checks point pickup
            if(map[(pacman.rect.y * w) + pacman.rect.x] == 3) {
                score += 10;
                total_points--;
                point_picked = true;
                map[(pacman.rect.y * w) + pacman.rect.x] = 0;
            }
            if(map[(pacman.rect.y * w) + pacman.rect.x + 1] == 3) {
                score += 10;
                total_points--;
                point_picked = true;
                map[(pacman.rect.y * w) + pacman.rect.x + 1] = 0;
            }
            if(map[((pacman.rect.y + 1) * w) + pacman.rect.x] == 3) {
                score += 10;
                total_points--;
                point_picked = true;
                map[((pacman.rect.y + 1) * w) + pacman.rect.x] = 0;
            }
            if(map[((pacman.rect.y + 1) * w) + pacman.rect.x + 1] == 3) {
                score += 10;
                total_points--;
                point_picked = true;
                map[((pacman.rect.y + 1) * w) + pacman.rect.x + 1] = 0;
            }

            // Checks boost pickup
            if(map[(pacman.rect.y * w) + pacman.rect.x] == 4) {
                score += 50;
                total_points--;
                boost_count--;
                point_picked = true;
                boosted = true;
                blinky.pursuing_plr = false;
                pinky.pursuing_plr = false;
                inky.pursuing_plr = false;
                clyde.pursuing_plr = false;
                boost_duration = 0;
                map[(pacman.rect.y * w) + pacman.rect.x] = 0;
            }
            if(map[(pacman.rect.y * w) + pacman.rect.x + 1] == 4) {
                score += 50;
                total_points--;
                boost_count--;
                point_picked = true;
                boosted = true;
                blinky.pursuing_plr = false;
                pinky.pursuing_plr = false;
                inky.pursuing_plr = false;
                clyde.pursuing_plr = false;
                boost_duration = 0;
                map[(pacman.rect.y * w) + pacman.rect.x + 1] = 0;
            }
            if(map[((pacman.rect.y + 1) * w) + pacman.rect.x] == 4) {
                score += 50;
                total_points--;
                boost_count--;
                point_picked = true;
                boosted = true;
                blinky.pursuing_plr = false;
                pinky.pursuing_plr = false;
                inky.pursuing_plr = false;
                clyde.pursuing_plr = false;
                boost_duration = 0;
                map[((pacman.rect.y + 1) * w) + pacman.rect.x] = 0;
            }
            if(map[((pacman.rect.y + 1) * w) + pacman.rect.x + 1] == 4) {
                score += 50;
                total_points--;
                boost_count--;
                point_picked = true;
                boosted = true;
                blinky.pursuing_plr = false;
                pinky.pursuing_plr = false;
                inky.pursuing_plr = false;
                clyde.pursuing_plr = false;
                boost_duration = 0;
                map[((pacman.rect.y + 1) * w) + pacman.rect.x + 1] = 0;
            }
            sprintf(score_str, "%d", score);

            pacman.rect.x += pacman.x_speed;
            pacman.rect.y += pacman.y_speed;
        }
        
    }
    
    printf("Game Over!\nYour score was %d points!\n", score);

    SDL_DestroyTexture(assets);
    SDL_FreeSurface(surf);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(arial);
    TTF_Quit();
    SDL_Quit();
    fclose(file);

    free(map);
    return 0;
}

// NOTES
/*
https://web.archive.org/web/20151006023914/http://blinky_spawn.comcast.net/~jpittman2/pacman/pacmandossier.html#Chapter_1
*/