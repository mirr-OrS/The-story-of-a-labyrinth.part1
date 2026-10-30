#include <stdio.h>
#include <ncurses.h>
#include <math.h>

#define MAP_SIZE 256
#define HALFPOINT 127
#define MAX_MENU_CHOICES 2

// in this section i define attributes a tile in the world can have
#define playerIsHere 64
#define NoGo 128
// models and all that for tiles will be specified in ID part, first 5 bits

typedef struct{
  short unsigned int ID;
  char *color;
}tile;
// each tile is 6x12

void createLabyrinth();
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

  WINDOW *menuWIN = newwin(9, 64, 0, 0); // newwin(height, width, starty, startx)
  if (menuWIN == NULL) {                 // >> Labyrinth Editor    (<-- highest possible
    endwin();                            //                           number of characters on the window)
    printf("Failed to create menu window. Check terminal dimensions.\n");
    return 1;
  }
  int choice = mainMenu(menuWIN);
  switch(choice){
    case 0:
      endwin();
      printf("Exiting the program... \033[?25h \n");
      return 0;
    
    case 1:
      createLabyrinth();
      break;

    case 2:
      // **[ startGame FUNCTION CALL ]**
      break;
    
    default:
      endwin();
      printf("NoN EXISTENT OPTION CHOSEN IN THE MENU\nExiting the program... \033[?25h \n");
      return 0;
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
  box(windw, 0, 0);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  wattron(windw, COLOR_PAIR(1) | A_BOLD);
  /*
     — — <( The Story of a Labyrinth )> — —
              -| MAIN MENU |-
      Play

   >> Labyrinth Editor

      Quit
  */
  int opt = 2; int ch = 0;
  char choices[MAX_MENU_CHOICES + 1][32] = {"Quit               ", "Labyrinth editor   ", "Play               "};
  char selecc[MAX_MENU_CHOICES + 1][32] = {"Quit       OwO bye!", "Labyrinth editor ;)", "Play      have fun!"};
  mvwprintw(windw,1, 13, "- - <( The Story of a Labyrinth )> - -");
  mvwprintw(windw, 2, 24, "-| MAIN MENU |-");  // window - row - column - string
  mvwprintw(windw, 3, 2, ">> "); wattron(windw, A_REVERSE); wprintw(windw, choices[MAX_MENU_CHOICES]); wattroff(windw, A_REVERSE);
  mvwprintw(windw, 5, 5, "Labyrinth Editor");
  mvwprintw(windw, 7, 5, "Quit");
  wrefresh(windw);
  while(ch != '\n'){
    ch = wgetch(windw);
    switch(ch){
      case KEY_UP:
        if(opt == MAX_MENU_CHOICES){
          opt = 0;
          mvwprintw(windw, 3, 2, "   "); wprintw(windw, "%s", choices[MAX_MENU_CHOICES]);
          mvwprintw(windw, 7, 2, ">> "); wattron(windw, A_REVERSE); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE);
          wrefresh(windw);
        } else{
          opt++;  // 7 - 2*opt
          mvwprintw(windw, (9 - (2 * opt)), 2, "   "); wprintw(windw, "%s", choices[opt - 1]);
          mvwprintw(windw, (7 - (2 * opt)), 2, ">> "); wattron(windw, A_REVERSE); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE);
          wrefresh(windw);
        } break;
      case KEY_DOWN:
        if(opt == 0){
          opt = MAX_MENU_CHOICES;
          mvwprintw(windw, 7, 2, "   "); wprintw(windw, "%s", choices[0]);
          mvwprintw(windw, 3, 2, ">> "); wattron(windw, A_REVERSE); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE);
          wrefresh(windw);
        } else{
          opt--;
          mvwprintw(windw, (5 - (2 * opt)), 2, "   "); wprintw(windw, "%s", choices[opt + 1]);
          mvwprintw(windw, (7 - (2 * opt)), 2, ">> "); wattron(windw, A_REVERSE); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE);
          wrefresh(windw);
        } break;
    }
  }
  delwin(windw);
  return opt;
}

void createLabyrinth(){
  short unsigned int labyrinth[64][64];
  int x, y;
  WINDOW *LABYRINTH = newwin(16, 64, 0, 0); // newwin(height, width, starty, startx)

  // 4. Check window creation
  // 5. Configure ncurses input
  // 6. Draw window border
  // 7. Refresh window
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