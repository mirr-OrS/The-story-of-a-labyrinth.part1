#include <stdio.h>
#include <ncurses.h>
#include <math.h>

#define MAP_SIZE 256
#define HALFPOINT 127

// in this section i define attributes a tile in the world can have
#define playerIsHere 64
#define NoGo 128
// models and all that for tiles will be specified in ID part, first 5 bits

typedef struct{
  short unsigned int ID;
  char *color;
}tile;
// each tile is 6x12

int mainMenu(WINDOW *windw);
void generateMap(unsigned char (*worldMap)[MAP_SIZE]);
void renderWorld(unsigned char (*worldMap)[MAP_SIZE], int playerX, int playerY);

int main(){

  printf("\033[?25l"); // hide cursor
  initscr();
  if(has_colors() == FALSE){
    endwin();
    printf("This terminal does not support colors! \033[?25h \n");
    return 1;
  }
  start_color(); raw(); noecho(); keypad(stdscr, TRUE);
  clear();

  WINDOW *menuWIN = newwin(8, 64, 1, 1); // newwin(height, width, starty, startx)
  if (menuWIN == NULL) {                 // >> Labyrinth Editor    (<-- highest possible
    endwin();                            //                           number of characters on the window)
    printf("Failed to create menu window. Check terminal dimensions.\n");
    return 1;
  }
  int choice = mainMenu(menuWIN);
  switch(choice){
    case 0:
      delwin(menuWIN); endwin();
      printf("Exiting the program... \033[?25h \n");
      return 0;
    
    default:
      break;
  }
  unsigned char worldMap[MAP_SIZE][MAP_SIZE] = {0};
  generateMap(worldMap);

  endwin();
  printf("\033[?25h"); // set cursor visibility back

  return 0;
}

void generateMap(unsigned char (*worldMap)[MAP_SIZE]){
  int dx = 0, dy = 0, distance = 0, ID = 0;

  for(int y = 0; y < MAP_SIZE; y++){
    for(int x = 0; x < MAP_SIZE; x++){
      dx = x - HALFPOINT;
      dy = y - HALFPOINT;

      distance = (int)sqrt(dx * dx + dy * dy);
      ID = distance % 64;

      switch(x){
        case 0:
        case 255:
          ID |= NoGo;
          break;
        
        default:
          break;
      } switch(y){
        case 0:
        case 255:
          ID |= NoGo;
          break;
        
        default:
          break;
      }   worldMap[y][x] = (unsigned char)ID;
    }
  }
}

int mainMenu(WINDOW *windw){
  keypad(windw, TRUE);
  //box(windw, 0, 0);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  wattron(windw, COLOR_PAIR(1));
  /*
     ——<( The Story of a Labyrinth )>——
              -| MAIN MENU |-
      Play

   >> Labyrinth Editor

      Quit
  */
  int opt = 2;
  mvwprintw(windw,0, 13, "- - <( The Story of a Labyrinth )> - -");
  mvwprintw(windw, 1, 24, "-| MAIN MENU |-");  // window - row - column - string
  mvwprintw(windw, 2, 1, " >> Play");
  mvwprintw(windw, 4, 4, "Labyrinth Editor");
  mvwprintw(windw, 6, 4, "Quit");
  wrefresh(windw);
  wgetch(windw);
  delwin(windw);
  return opt;
}

void renderWorld(unsigned char (*worldMap)[MAP_SIZE], int playerX, int playerY){
  // does nothing and dies
}

/*
3 character models needed
main character, generic NPC and Minotaur

3x3
                      # #
 #                     #
###                   ###

NPC and MC             Minotuar
*/