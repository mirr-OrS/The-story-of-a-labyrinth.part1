#include <stdio.h>

// in this section i define attributes a tile in the world can have
#define tree 32
#define NoGo 64
#define playerIsHere 128
// models and all that for tiles will be specified in ID part, first 5 bits

void renderWorld(unsigned char (*worldMap)[], int playerX, int playerY);

int main(){
  unsigned char worldMap[256][256] = {0}; // write your world i guess
  return 0;
}
