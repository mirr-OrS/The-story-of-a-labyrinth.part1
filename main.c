#include <stdio.h>
#include <ncurses.h>
#include <math.h>

#define MAP_SIZE 256
#define HALFPOINT 127
#define EDITOR_SIZE 64
#define MAX_MENU_CHOICES 2
#define VIEW_WIDTH  32
#define VIEW_HEIGHT 16

// color section, i define colors here :)
#define COLOR_FLOOR 8
#define COLOR_WALL 9
// #define COLOR_PIT 10 not needed, black already exists :D
#define COLOR_TRAP 11

// in this section i define attributes a tile in the world can have
#define playerIsHere 64
#define NoGo 128
// models and all that for tiles will be specified in ID part, first 5 bits

// this is to avoid confusion (my own :P) in the renderLabrnth() function
#define Floor 1
#define Wall 2
#define Pit 3
#define Trap 4

typedef enum{
  FLOOR,
  WALL,
  PIT,
  TRAP               // for now TRAP is generic, I'll implement and distinguish them later
}labrnthTILE_t;      // to be precise: it's a floor tile with stuff that damages u!   >:)

typedef struct{
  labrnthTILE_t type;
}editorTILE_t;

typedef struct{
  editorTILE_t tile[EDITOR_SIZE][EDITOR_SIZE];
}maze_t;

typedef struct{
  short unsigned int ID;
  char *color;
}tile;
// each tile is 6x12

void renderLabrnth(WINDOW *windw, maze_t *labyrinth, int curseX, int curseY, int *viewX, int *viewY);
void createLabyrinth();
int mainMenu(WINDOW *windw);
void updView(int *viewY, int *viewX, int curseY, int curseX);
void mvwprintpx(WINDOW *windw, int x, int y, int colorPair, char decor);
void mvwprintcrs(WINDOW *windw, int y, int x, int colorPair);
void makeLegend(WINDOW *windw);
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
  init_color(COLOR_FLOOR, 404,  490, 718); init_pair(Floor, COLOR_BLACK,  COLOR_FLOOR);
  init_color(COLOR_WALL,  271,  294, 400); init_pair(Wall,  COLOR_BLACK,  COLOR_WALL);
  /*  Stuff to make this evenly spaced  */ init_pair(Pit,   COLOR_WHITE,  COLOR_BLACK);
  init_color(COLOR_TRAP,  102, 1000, 835); init_pair(Trap,  COLOR_BLACK,  COLOR_TRAP);
  /*  Stuff to make this evenly spaced  */ init_pair(5,     COLOR_GREEN,   COLOR_BLACK);

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
      printf("NoN EXISTENT OPTION CHOSEN IN THE MENU (How did u do that?)\nExiting the program... \033[?25h \n");
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

      distance = (int) sqrt(dx * dx + dy * dy);
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
  wattron(windw, COLOR_PAIR(5));
  /*
     — — <( The Story of a Labyrinth )> — —
              -| MAIN MENU |-

      Play

   >> Labyrinth Editor

      Quit
  */
  int opt = 2; int ch = 0;
  char choices[MAX_MENU_CHOICES + 1][32] = {"Quit                ", "Labyrinth editor    ", "Play                "};
  char selecc[MAX_MENU_CHOICES + 1][32] = {"Quit        OwO bye!", "Labyrinth editor  ;)", "Play       have fun!"};
  mvwprintw(windw, 0, 13, "- - <( The Story of a Labyrinth )> - -");
  mvwprintw(windw, 1, 24, "-| MAIN MENU |-");  // window - row - column - string
  mvwprintw(windw, 3, 1, ">> "); wattron(windw, A_REVERSE | A_BOLD); wprintw(windw, choices[MAX_MENU_CHOICES]); wattroff(windw, A_REVERSE | A_BOLD);
  mvwprintw(windw, 5, 4, "Labyrinth editor");
  mvwprintw(windw, 7, 4, "Quit");
  wrefresh(windw);
  while(ch != '\n'){
    ch = wgetch(windw);
    switch(ch){
      case KEY_UP:
        if(opt == MAX_MENU_CHOICES){
          opt = 0;
          mvwprintw(windw, 3, 1, "   "); wprintw(windw, "%s", choices[MAX_MENU_CHOICES]);
          mvwprintw(windw, 7, 1, ">> "); wattron(windw, A_REVERSE | A_BOLD); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE | A_BOLD);
          wrefresh(windw);
        } else{
          opt++;  // 7 - 2*opt
          mvwprintw(windw, (9 - (2 * opt)), 1, "   "); wprintw(windw, "%s", choices[opt - 1]);
          mvwprintw(windw, (7 - (2 * opt)), 1, ">> "); wattron(windw, A_REVERSE | A_BOLD); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE | A_BOLD);
          wrefresh(windw);
        } break;
      case KEY_DOWN:
        if(opt == 0){
          opt = MAX_MENU_CHOICES;
          mvwprintw(windw, 7, 1, "   "); wprintw(windw, "%s", choices[0]);
          mvwprintw(windw, 3, 1, ">> "); wattron(windw, A_REVERSE | A_BOLD); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE | A_BOLD);
          wrefresh(windw);
        } else{
          opt--;
          mvwprintw(windw, (5 - (2 * opt)), 1, "   "); wprintw(windw, "%s", choices[opt + 1]);
          mvwprintw(windw, (7 - (2 * opt)), 1, ">> "); wattron(windw, A_REVERSE | A_BOLD); wprintw(windw, "%s", selecc[opt]); wattroff(windw, A_REVERSE | A_BOLD);
          wrefresh(windw);
        } break;
    }
  }
  delwin(windw);
  return opt;
}

void mvwprintpx(WINDOW *windw, int y, int x, int colorPair, char decor){
  wmove(windw, y, 2 * x);
  wattron(windw, COLOR_PAIR(colorPair)); // turn on bacground color, foreground is mostly useless
  wprintw(windw, "%c%c", decor, decor); // 2 spaces make a pixel and background is its color
  wattroff(windw, COLOR_PAIR(colorPair)); // turn off bacground color
}

void mvwprintcrs(WINDOW *windw, int y, int x, int colorPair){
  wmove(windw, y, 2 * x);
  wattron(windw, COLOR_PAIR(colorPair)); // turn on bacground color, foreground is mostly useless
  wprintw(windw, "::"); // 2 spaces make a pixel and background is its color
  wattroff(windw, COLOR_PAIR(colorPair)); // turn off bacground color
}

void createLabyrinth(){
  maze_t labyrinth = {0}; // 0 = floor, 1 = wall, wait, I'll make an enum... (it was a short unsigned int array before)
  int curseX = 0, curseY = 0, viewX = 0, viewY = 0;
  WINDOW *labrnthWIN = newwin(16, 64, 0, 0); // newwin(height, width, starty, startx)
  WINDOW *legend = newwin(10, 56, 16, 4);
  if(labrnthWIN == NULL || legend == NULL){
    printw("Failed creating the new window... XP !"); // XP is closed eyes with tongue out, like dead cartoon characters
    refresh();
    return;
  }
  makeLegend(legend);
  keypad(labrnthWIN, TRUE);
  int ch = 0; short unsigned int saved = FALSE; char name[64] = "\0";
  while(TRUE){

    renderLabrnth(labrnthWIN, &labyrinth, curseX, curseY, &viewX, &viewY);
    ch = wgetch(labrnthWIN);

    switch(ch){
      case 'w':
        if(viewY > 0) viewY--;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case 's':
        if(viewY < (EDITOR_SIZE - 1)) viewY++;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case 'a':
        if(viewX > 0) viewX--;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case 'd':
        if(viewX < (EDITOR_SIZE - 1)) viewX++;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case KEY_UP:
        if(curseY > 0) curseY--;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case KEY_DOWN:
        if(curseY < (EDITOR_SIZE - 1)) curseY++;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case KEY_LEFT:
        if(curseX > 0) curseX--;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case KEY_RIGHT:
        if(curseX < (EDITOR_SIZE - 1)) curseX++;
        updView(&viewY, &viewX, curseY, curseX);
        break;

      case '\033':
        if(!saved){ // if not saved
          // ARE YOU SURE SHIRLEY?
        }
        delwin(labrnthWIN);
        return;

      case 'z':
        labyrinth.tile[curseY][curseX].type = FLOOR;
        saved = FALSE;
        break;

      case 'x':
        labyrinth.tile[curseY][curseX].type = WALL;
        saved = FALSE;
        break;

      case 'c':
        labyrinth.tile[curseY][curseX].type = PIT;
        saved = FALSE;
        break;

      case 'v':
        labyrinth.tile[curseY][curseX].type = TRAP;
        saved = FALSE;
        break;
      
      case '-':
        if(name[0] == '\0'){
          // ask with what name to save
        } // pretend I use save(name which saves to a file)
        saved = TRUE;
    }
  }
}

void renderLabrnth(WINDOW *windw, maze_t *labyrinth, int curseX, int curseY, int *viewX, int *viewY){
  int screenX = 0, screenY = 0;
  editorTILE_t currntTile; int colorPair = 0;
  for(int y = *viewY; y < (VIEW_HEIGHT + *viewY); y++){
    for(int x = *viewX; x < (VIEW_WIDTH + *viewX); x++){
      currntTile.type = labyrinth -> tile[y][x].type;
      switch(currntTile.type){
        case FLOOR: colorPair = Floor; break;
        case WALL : colorPair = Wall ; break;
        case PIT  : colorPair = Pit  ; break;
        case TRAP : colorPair = Trap ; break;
      }
      
      if(x == curseX && y == curseY){
        mvwprintcrs(windw, screenY, screenX, colorPair);
      } else mvwprintpx(windw, screenY, screenX, colorPair, ' ');

      screenX++;
      if(screenX == VIEW_WIDTH){
        screenX = 0; screenY++;
      } wrefresh(windw);
    }
  }
/*
----{ NOTES ON VIEWPORT }----

viewX/Y is where you start drawing the screen from (ABSOLUTE POSITION)

curseX/Y is where the cursor is,
you draw it at ((curseX - viewX); (curseY - viewY)) coordinates
on the screen.

screenX/Y is a generic variable that calculates the position of objects on screen
formulas are: screenX = curseX - viewX
              screenY = curseY - viewY

when drawing stuff on terminal through screenX/Y remember to do screenX * 2
because YOU said 2 cells == one pixel :P
*/
}

void updView(int *viewY, int *viewX, int curseY, int curseX){
  if(curseX >= (*viewX + VIEW_WIDTH - 1)){
    *viewX = curseX - VIEW_WIDTH + 1;
  } if(curseX <= *viewX){
    *viewX = curseX;
  }
  if(curseY >= (*viewY + VIEW_HEIGHT - 1)){
    *viewY = curseY - VIEW_HEIGHT + 1;
  } if(curseY <= *viewY){
    *viewY = curseY;
  }
}

void makeLegend(WINDOW *windw){
/*
                    - -- -- --- { LEGEND } --- -- -- -
  MOVE CURSOR [::] WITH [ARROW KEYS]  MOVE SCREEN WITH [CTRL + ARROW KEYS]

   Z: place Floor [FLOOR TILE COLOR]   C: place Trap [TRAP TILE COLOR]

   X: place Wall [WALL  TILE  COLOR]   V: place Pit [PIT  TILE  COLOR]
*/
  box(windw, 0, 0);
  mvwprintw(windw, 1, 10, "- -- -- --- { LEGEND } --- -- -- -");
  mvwprintw(windw, 2, 6, "    MOVE CURSOR [::] WITH [ARROW KEYS]");
  mvwprintw(windw, 4, 5, "Z: place Floor []          C: place Trap []");
  mvwprintw(windw, 5, 5, "ESC: Quit                  '-': Save labyrinth");
  mvwprintw(windw, 6, 5, "X: place Wall  []          V: place Pit  []");
  mvwprintw(windw, 8, 6, "   MOVE  SCREEN  WITH  [W   A   S   D]");
  mvwprintpx(windw, 4, 10, Floor, ' '); mvwprintpx(windw, 4, 23, Trap, ' ');
  mvwprintpx(windw, 6, 10, Wall,  ' '); mvwprintpx(windw, 6, 23,  Pit,  ' ');
  wrefresh(windw);
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