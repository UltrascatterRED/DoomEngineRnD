//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\        NOSTALGIA FPS Builder       //
//        by UltrascatterRED          \\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

// Requisite libraries
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <GL/glut.h>
#include "GameStructs.h"
#include "Debug.h"
#include "Render.h"

// Keymaps for player/camera controls
// DEV NOTE: Look into refactor to constant/readonly struct for organization
unsigned char MOVE_FORWARD = 'w';
unsigned char MOVE_BACK = 's';
unsigned char STRAFE_LEFT = 'a';
unsigned char STRAFE_RIGHT = 'd';
unsigned char LOOK_LEFT = ',';
unsigned char LOOK_RIGHT = '.';
unsigned char FLY_UP = 'e';
unsigned char FLY_DOWN = 'q';

// Instantiate game structs (from GameStructs.h)

void execInputsDebug()
{
  if(KeyState.moveF == 1)
  {
    printf("Pressing %c: moving forward\n", MOVE_FORWARD);
  }
  if(KeyState.moveB == 1)
  {
    printf("Pressing %c: moving back\n", MOVE_BACK);
  }
  if(KeyState.strafeL == 1)
  {
    printf("Pressing %c: strafing left\n", STRAFE_LEFT);
  }
  if(KeyState.strafeR == 1)
  {
    printf("Pressing %c: strafing right\n", STRAFE_RIGHT);
  }
  if(KeyState.lookL == 1)
  {
    printf("Pressing %c: looking left\n", LOOK_LEFT);
  }
  if(KeyState.lookR == 1)
  {
    printf("Pressing %c: looking right\n", LOOK_RIGHT);
  }
  if(KeyState.flyU == 1)
  {
    printf("Pressing %c: flying up\n", FLY_UP);
  }
  if(KeyState.flyD == 1)
  {
    printf("Pressing %c: flying down\n", FLY_DOWN);
  }
}

// checks for updates to input keys' state and performs corresponding action(s)
// (i.e. MOVE_FORWARD key pressed down, player/camera moves forward)
void execInputs()
{
  // misc input parameters
  int lookSpeed = 4; // speed of looking left/right
  int moveSpeed = 10; // speed of moving around the environment
  int flySpeed = 4; // speed of flying up/down  
  // deltaX and deltaY represent the player's absolute displacement
  // based on what direction they are looking/moving in. Used to
  // greatly simplify movement calculation.
  int deltaX = TrigVals.sin[Player.angle] * moveSpeed;
  int deltaY = TrigVals.cos[Player.angle] * moveSpeed;

  // move forward; represented by positive change in both deltaX and
  // deltaY
  if(KeyState.moveF == 1)
  {
    Player.x += deltaX;
    Player.y += deltaY;
  }
  // move backward; represented by negative change in both deltaX and
  // deltaY
  if(KeyState.moveB == 1)
  {
    Player.x -= deltaX;
    Player.y -= deltaY;
  }
  // strafe left; represented by inverting displacement of player x and y.
  // this inversion happens due to movement along a vector that is offset
  // 90 degrees from looking direction
  if(KeyState.strafeL == 1)
  {
    Player.x -= deltaY;
    Player.y += deltaX;
  }
  // strafe right; represented by inverting displacement of player x and y.
  // this inversion happens due to movement along a vector that is offset
  // -90 degrees from looking direction
  if(KeyState.strafeR == 1)
  {
    Player.x += deltaY;
    Player.y -= deltaX;
  }
  if(KeyState.lookL == 1)
  {
    Player.angle -= lookSpeed;
    if(Player.angle < 0)
    {
      Player.angle += 360;
    }
  }
  if(KeyState.lookR == 1)
  {
    Player.angle += lookSpeed;
    if(Player.angle > 359)
    {
      Player.angle -= 360;
    }
  }
  if(KeyState.flyU == 1)
  {
    Player.z -= flySpeed;
  }
  if(KeyState.flyD == 1)
  {
    Player.z += flySpeed;
  }

}

void displayFrame()
{
  // check if it's time to draw next frame
  if(Bft.frame1-Bft.frame2 >= MSPF)
  {
    clearBackground();
    execInputs();
    // execInputsDebug(); // debug (duh)
    //drawTest(); // debug; must comment out drawView() to use 
    drawView();
    // frame2 holds elapsed time (ms) at which last frame was drawn;
    // frame2 is continuously used to calculate when to draw next frame
    Bft.frame2 = Bft.frame1;
    glutSwapBuffers();
    glutReshapeWindow(GL_WIN_WIDTH, GL_WIN_HEIGHT); // prevents window scaling
  }
  // time elapsed in milliseconds since engine started
  Bft.frame1 = glutGet(GLUT_ELAPSED_TIME);
  glutPostRedisplay();
  // debug
  //printFPS(&Bft);
}

// glut callback function; checks if any new keys have been pressed down,
// writes updates to global struct KeyState
void checkKeysDown(unsigned char key, int x, int y)
{
  /*unsigned char MOVE_FORWARD = 'w';*/
  /*unsigned char MOVE_BACK = 's';*/
  /*unsigned char STRAFE_LEFT = 'a';*/
  /*unsigned char STRAFE_RIGHT = 'd';*/
  /*unsigned char LOOK_LEFT = ',';*/
  /*unsigned char LOOK_RIGHT = '.';*/
  /*unsigned char FLY_UP = 'e';*/
  /*unsigned char FLY_DOWN = 'q';*/
  /*int moveF, moveB, strafeL, strafeR, lookL, lookR, flyU, flyD;*/
  // DEV NOTE: "== 1" part of expression maay be redundant, test without it
  if(key == MOVE_FORWARD == 1)  { KeyState.moveF = 1; }
  if(key == MOVE_BACK == 1)     { KeyState.moveB = 1; }
  if(key == STRAFE_LEFT == 1)   { KeyState.strafeL = 1; }
  if(key == STRAFE_RIGHT == 1)  { KeyState.strafeR = 1; }
  if(key == LOOK_LEFT == 1)     { KeyState.lookL = 1; }
  if(key == LOOK_RIGHT == 1)    { KeyState.lookR = 1; }
  if(key == FLY_UP == 1)        { KeyState.flyU = 1; }
  if(key == FLY_DOWN == 1)      { KeyState.flyD = 1; }
}
// glut callback function; checks if any new keys have been released, 
// writes updates to global struct KeyState
void checkKeysUp(unsigned char key, int x, int y)
{
  if(key == MOVE_FORWARD == 1)  { KeyState.moveF = 0; }
  if(key == MOVE_BACK == 1)     { KeyState.moveB = 0; }
  if(key == STRAFE_LEFT == 1)   { KeyState.strafeL = 0; }
  if(key == STRAFE_RIGHT == 1)  { KeyState.strafeR = 0; }
  if(key == LOOK_LEFT == 1)     { KeyState.lookL = 0; }
  if(key == LOOK_RIGHT == 1)    { KeyState.lookR = 0; }
  if(key == FLY_UP == 1)        { KeyState.flyU = 0; }
  if(key == FLY_DOWN == 1)      { KeyState.flyD = 0; }
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(GL_WIN_WIDTH, GL_WIN_HEIGHT);
  glutCreateWindow(""); 
  glPointSize(PIXEL_SCALE);                        //pixel size
  gluOrtho2D(0,GL_WIN_WIDTH,0,GL_WIN_HEIGHT);      //origin bottom left
  init();
  glutDisplayFunc(displayFrame);
  glutKeyboardFunc(checkKeysDown);
  glutKeyboardUpFunc(checkKeysUp);
  glutMainLoop();
  return 0;
}
