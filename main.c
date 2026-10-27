#include <stdio.h>
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

void generateMap(unsigned char (*worldMap)[MAP_SIZE]);
void renderWorld(unsigned char (*worldMap)[MAP_SIZE], int playerX, int playerY);

int main(){
  unsigned char worldMap[MAP_SIZE][MAP_SIZE] = {0}; // write your world i guess
  generateMap(worldMap);
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
(change color scheme)
*/