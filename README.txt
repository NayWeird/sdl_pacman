Projekt: SDL Pacman
Autor: Konrád Neuvirt
Jak přeložit: gcc main.c sdl.c entities.c -lSDL2 -lSDL2_image -lSDL2_ttf -fsanitize=address -lm
              nebo jednoduše přikazem 'make'
Synopse: Pacmana lze ovládat klávesami w,a,s,d.
         Pacman může přijít o život tím že se střetne s kterýmkoliv duchem.
         Poté co pacman ztratí všechny 3 životy se program vypne a v terminálu se vypíše finální skóre.
         Program se rovněž vypne poté co pacman posbírá všechny body na mapě.
         Z mapy jsou náhodně vybrány 4 speciální body.
         Když pacman takový bod sní, na krátkou dobu může jíst duchy.
         Na mapě jsou 4 duchové kteří se snaží pacmana lovit.
         Blinky (červený) jde přímo po pacmanovi.
         Pinky (růžový) se snaží dostat před pacmana.
         Inky (tyrkysový) je poněkud nepředvídatelný.
         Clyde (oranžový) se snaží být blízko pacmanovi, ale ne moc blízko.
         Kromě souborů obsahující program je také potřeba mít k dispozici textový soubor s herní mapou a asset mapu.

--------

Project: SDL Pacman
Author: Konrád Neuvirt
Compilation: gcc main.c sdl.c entities.c -lSDL2 -lSDL2_image -lSDL2_ttf -fsanitize=address -lm
             or simply use 'make' command
Synopsis: Pacman can be controlled using w,a,s,d keys.
          Pacman will lose a life by colliding with any ghost.
          Once pacman will have lost all 3 of his lives, the program will shut down and the final score will be displayed in the terminal.
          The program will also shut down once all the points on the map have been collected.
          There are 4 points randomly chosen on the map.
          Once pacman eats one such point, he will be able to eat the ghosts for a short duration.
          There are 4 ghosts on the map who try to hunt pacman down.
          Blinky (red) is in direct pursuit of pacman.
          Pinky (pinky) tries to get ahead of pacman to corner him.
          Inky (cyan) is perhaps the most unpredictable ghost.
          Clyde (orange) tries to stick close to pacman, but not too close.
          Besides the program files, a text file containing the map as well as an asset map are required.