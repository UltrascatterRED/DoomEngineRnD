#ifndef GAMESTRUCTS_H
#define GAMESTRUCTS_H

typedef struct
{
  int frame1, frame2;
}bufferTime;
// stores keyboard input state
typedef struct
{
  // move forward, move back, strafe left, strafe right, look left, look right, fly up, fly down
  int moveF, moveB, strafeL, strafeR, lookL, lookR, flyU, flyD;
}keyState;
// stores pre-calculated sine and cosine values to avoid calculating at 
// runtime 
typedef struct
{
  float sin[360];
  float cos[360];
}trigVals;
// player state information
typedef struct
{
  int x, y, z; // 3D position; z is up/down
  int angle; // angle of player's looking direction
}player;

typedef struct
{
  int x1;
  int y1;
  int x2;
  int y2;
  int height; // height in z direction
  int color;  // wall color lookup code
}wall;

void init(); // initializes static structs to necessary values
extern bufferTime Bft;
extern keyState KeyState;
extern trigVals TrigVals;
extern player Player;

#endif
