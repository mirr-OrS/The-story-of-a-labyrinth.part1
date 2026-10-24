#include <stdio.h>

// in this section i define attributes a tile in the world can have
#define playerIsHere 64
#define NoGo 128
// models and all that for tiles will be specified in ID part, first 5 bits

typedef struct{
  short unsigned int ID;
  char *color;
}tile;
// each tile is 6x12

void renderWorld(unsigned char (*worldMap)[256], int playerX, int playerY);

int main(){
  unsigned char worldMap[256][256] = {0}; // write your world i guess
  return 0;
}

void renderWorld(unsigned char (*worldMap)[256], int playerX, int playerY){
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
(change color scheme)
*/