#include "entities.h"

// Blinkys AI is simple, he just tries to follow pacman
int get_blinkys_direction(int *map, int w, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction) {
    double min = 64;
    int id = 0;
    double distance[4];    // 0 - right, 1 - left, 2 - up, 3 - down
    bool blocked = false;

    distance[0] = sqrt(pow(abs((ghost_rect->x + 1) - plr->x), 2) + pow(abs(ghost_rect->y - plr->y), 2));  // Right
    distance[1] = sqrt(pow(abs((ghost_rect->x - 1) - plr->x), 2) + pow(abs(ghost_rect->y - plr->y), 2));  // Left
    distance[2] = sqrt(pow(abs(ghost_rect->x - plr->x), 2) + pow(abs((ghost_rect->y - 1) - plr->y), 2));  // Up
    distance[3] = sqrt(pow(abs(ghost_rect->x - plr->x), 2) + pow(abs((ghost_rect->y + 1) - plr->y), 2));  // Down

    for(int i = 0; i < 4; i++) {
        if(distance[i] < min) {
            min = distance[i];
            id = i;
        }
    }
    
    switch(current_direction) {
        case 1: // Right
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                return current_direction;
            }
            break;
        case 2: // Left
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                return current_direction;
            }
            break;
        case 3: // Up
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] == 1) {
                return 0;
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
        case 4: // Down
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 2)) {
                return 0;
            }
            if(map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
    }

}

// Inkys' AI is the most complex, his target is a the tip of a vector originating from blinky and passing through pacmans' future position (the same position pinky tries to get to)
int get_inkys_direction(int *map, int w, int x_speed, int y_speed, SDL_Rect *plr, SDL_Rect *blinky, SDL_Rect *ghost_rect, int current_direction) {
    double min = 64;
    int id = 0;
    double distance[4];    // 0 - right, 1 - left, 2 - up, 3 - down
    bool blocked = false;
    int future_x = x_speed * 3;
    int future_y = y_speed * 3;

    distance[0] = sqrt(pow(abs((blinky->x + 1) - (plr->x + future_x)) * 2, 2) + pow(abs(blinky->y - (plr->y + future_y)) * 2, 2));  // Right
    distance[1] = sqrt(pow(abs((blinky->x - 1) - (plr->x + future_x)) * 2, 2) + pow(abs(blinky->y - (plr->y + future_y)) * 2, 2));  // Left
    distance[2] = sqrt(pow(abs(blinky->x - (plr->x + future_x)) * 2, 2) + pow(abs((blinky->y - 1) - (plr->y + future_y)) * 2, 2));  // Up
    distance[3] = sqrt(pow(abs(blinky->x - (plr->x + future_x)) * 2, 2) + pow(abs((blinky->y + 1) - (plr->y + future_y)) * 2, 2));  // Down

    for(int i = 0; i < 4; i++) {
        if(distance[i] < min) {
            min = distance[i];
            id = i;
        }
    }
    
    
    switch(current_direction) {
        case 1: // Right
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                return current_direction;
            }
            break;
        case 2: // Left
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                return current_direction;
            }
            break;
        case 3: // Up
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] == 1) {
                return 0;
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
        case 4: // Down
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 2)) {
                return 0;
            }
            if(map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
    }

}

// Pinky tries to get ahead of pacman to ambush him
int get_pinkys_direction(int *map, int w, int x_speed, int y_speed, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction) {
    double min = 64;
    int id = 0;
    double distance[4];    // 0 - right, 1 - left, 2 - up, 3 - down
    bool blocked = false;
    int future_x = x_speed * 3;
    int future_y = y_speed * 3;

    distance[0] = sqrt(pow(abs((ghost_rect->x + 1) - (plr->x + future_x)), 2) + pow(abs(ghost_rect->y - (plr->y + future_y)), 2));  // Right
    distance[1] = sqrt(pow(abs((ghost_rect->x - 1) - (plr->x + future_x)), 2) + pow(abs(ghost_rect->y - (plr->y + future_y)), 2));  // Left
    distance[2] = sqrt(pow(abs(ghost_rect->x - (plr->x + future_x)), 2) + pow(abs((ghost_rect->y - 1) - (plr->y + future_y)), 2));  // Up
    distance[3] = sqrt(pow(abs(ghost_rect->x - (plr->x + future_x)), 2) + pow(abs((ghost_rect->y + 1) - (plr->y + future_y)), 2));  // Down

    for(int i = 0; i < 4; i++) {
        if(distance[i] < min) {
            min = distance[i];
            id = i; // id + 1 = desired direction
        }
    }
    
    
    switch(current_direction) {
        case 1: // Right
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                return current_direction;
            }
            break;
        case 2: // Left
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                return current_direction;
            }
            break;
        case 3: // Up
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] == 1) {
                return 0;
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
        case 4: // Down
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 2)) {
                return 0;
            }
            if(map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
    }

}

// Clyde follows pacman, but if he gets too close he retreats and he resumes his hunt when far enough
int get_clydes_direction(int *map, int w, SDL_Point *alt_target, SDL_Rect *plr, SDL_Rect *ghost_rect, int current_direction) {
    double min = 64;
    int id = 0;
    double distance[4];    // 0 - right, 1 - left, 2 - up, 3 - down
    double distance_home[4];
    bool blocked = false;

    distance[0] = sqrt(pow(abs((ghost_rect->x + 1) - plr->x), 2) + pow(abs(ghost_rect->y - plr->y), 2));  // Right
    distance[1] = sqrt(pow(abs((ghost_rect->x - 1) - plr->x), 2) + pow(abs(ghost_rect->y - plr->y), 2));  // Left
    distance[2] = sqrt(pow(abs(ghost_rect->x - plr->x), 2) + pow(abs((ghost_rect->y - 1) - plr->y), 2));  // Up
    distance[3] = sqrt(pow(abs(ghost_rect->x - plr->x), 2) + pow(abs((ghost_rect->y + 1) - plr->y), 2));  // Down

    distance_home[0] = sqrt(pow(abs((ghost_rect->x + 1) - alt_target->x), 2) + pow(abs(ghost_rect->y - alt_target->y), 2));  // Right
    distance_home[1] = sqrt(pow(abs((ghost_rect->x - 1) - alt_target->x), 2) + pow(abs(ghost_rect->y - alt_target->y), 2));  // Left
    distance_home[2] = sqrt(pow(abs(ghost_rect->x - alt_target->x), 2) + pow(abs((ghost_rect->y - 1) - alt_target->y), 2));  // Up
    distance_home[3] = sqrt(pow(abs(ghost_rect->x - alt_target->x), 2) + pow(abs((ghost_rect->y + 1) - alt_target->y), 2));  // Down

    for(int i = 0; i < 4; i++) {
        if(distance[i] < min) {
            min = distance[i];
            id = i;
        }
    }
    
    if(distance[id] < 7) {
        //printf("calm\n");
        min = 64;
        for(int i = 0; i < 4; i++) {
            distance[i] = distance_home[i];
            if(distance[i] < min) {
                min = distance[i];
                id = i;
            }
        }
    }
    /*else {
        printf("angery\n");
    }*/

    switch(current_direction) {
        case 1: // Right
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                return current_direction;
            }
            break;
        case 2: // Left
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                if(id == 2) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 2)) {
                if(id == 3) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] == 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] == 1) {
                return 0;
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                return current_direction;
            }
            break;
        case 3: // Up
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] == 1) {
                return 0;
            }
            if(map[((ghost_rect->y - 1) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y - 1) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
        case 4: // Down
            if(map[(ghost_rect->y * w) + ghost_rect->x - 1] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x - 1] != 1) {
                if(id == 1) {
                    return id + 1;
                }
            }
            if(map[(ghost_rect->y * w) + ghost_rect->x + 2] != 1 && map[((ghost_rect->y + 1) * w) + ghost_rect->x + 2] != 1) {
                if(id == 0) {
                    return id + 1;
                }
            }
            if((map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x] == 2) && (map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] == 2)) {
                return 0;
            }
            if(map[((ghost_rect->y + 2) * w) + ghost_rect->x] != 1 && map[((ghost_rect->y + 2) * w) + ghost_rect->x + 1] != 1) {
                return current_direction;
            }
            break;
    }

}

// Checks if two entities have collided
bool collided(int *map, int w, SDL_Rect *plr, SDL_Rect *ghost) {
    int plr_pos = (plr->y * w) + plr->x;
    if((ghost->y * w) + ghost->x == plr_pos) {
        return true;
    }
    if((ghost->y * w) + ghost->x + 1 == plr_pos) {
        return true;
    }
    if(((ghost->y + 1) * w) + ghost->x == plr_pos) {
        return true;
    }
    if(((ghost->y + 1) * w) + ghost->x + 1 == plr_pos) {
        return true;
    }
    if(((ghost->y) * w) + ghost->x - 1 == plr_pos) {
        return true;
    }
    if(((ghost->y + 1) * w) + ghost->x - 1 == plr_pos) {
        return true;
    }
    if(((ghost->y - 1) * w) + ghost->x == plr_pos) {
        return true;
    }
    if(((ghost->y - 1) * w) + ghost->x + 1 == plr_pos) {
        return true;
    }
    return false;
}

// Checks wheter an entity is passing through a portal and changes it's position if so (only written for the horizontal axis)
void portal_travel(int w, SDL_Rect *plr, SDL_Rect *r, SDL_Rect *p, SDL_Rect *c, SDL_Rect *o) {
    if(plr->x == 0) {
        plr->x = w - 3;
    }
    if(plr->x == w - 2) {
        plr->x = 0;
    }
    if(r->x == 0) {
        r->x = w - 3;
    }
    if(r->x == w - 2) {
        r->x = 0;
    }
    if(p->x == 0) {
        p->x = w - 3;
    }
    if(p->x == w - 2) {
        p->x = 0;
    }
    if(c->x == 0) {
        c->x = w - 3;
    }
    if(c->x == w - 2) {
        c->x = 0;
    }
    if(o->x == 0) {
        o->x = w - 3;
    }
    if(o->x == w - 2) {
        o->x = 0;
    }
}