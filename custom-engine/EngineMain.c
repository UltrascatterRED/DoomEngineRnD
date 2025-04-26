//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\        DOOM-like Game Engine       //
//        by UltrascatterRED          \\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//       

// Requisite libraries
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <GL/glut.h>

// Defines global constants and structs

// Screen width and height are measured in engine-defined pixels.
// That is, the scaled pixels that the engine draws, which may differ
// in size from the actual, physical ones on the monitor.
#define SCREEN_WIDTH    200     // screen width in engine pixels
#define SCREEN_HEIGHT   150     // screen height in engine pixels
#define PIXEL_SCALE     4
// True width and height of the output window in monitor pixels
#define GL_WIN_WIDTH    (SCREEN_WIDTH*PIXEL_SCALE)
#define GL_WIN_HEIGHT   (SCREEN_HEIGHT*PIXEL_SCALE)
#define MSPF            50      // milliseconds per frame; 1000/MSPF = FPS

// DEBUG: counts the number of times displayFrame() has executed. 
// Used to display FPS at regular intervals.
int displayFrame_Counter = 0; 
// how many executions of displayFrame() between FPS printouts; increase to reduce rate of printouts
unsigned int fpsDisplayRate = 100000; // default: 100000 
// number of FPS samples to average from per FPS display; do not set too high, lest you gobble up memory with only integers
int fpsSampleRate = 100; // default: 100
// running total of FPS samples, used to calculate output average
int sampleSum = 0;
// tracks index to insert next FPS sample to
int nextSampleIdx = 0;
// END DEBUG
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

// frame buffer information; used to draw frames
typedef struct
{
  int frame1, frame2;
}bufferTime; bufferTime Bft;
// stores keyboard input state
typedef struct
{
  // move forward, move back, strafe left, strafe right, look left, look right, fly up, fly down
  int moveF, moveB, strafeL, strafeR, lookL, lookR, flyU, flyD;
}keyState; keyState KeyState;
// stores pre-calculated sine and cosine values to avoid calculating at 
// runtime 
typedef struct
{
  float sin[360];
  float cos[360];
}trigVals; trigVals TrigVals;
// player state information
typedef struct
{
  int x, y, z; // 3D position; z is up/down
  int angle; // angle of player's looking direction
}player; player Player;

void drawPixel(int x, int y, int color)
{
  // rgb[0] is red
  // rgb[1] is green
  // rgb[2] is blue
  int rgb[3];
  switch (color) 
  {
    case 0:
      // pure red
      rgb[0]=255; rgb[1]=0; rgb[2]=0;
      break;
    case 1:
      // dark red
      rgb[0]=160; rgb[1]=0; rgb[2]=0;
      break;
    case 2:
      // pure green
      rgb[0]=0; rgb[1]=255; rgb[2]=0;
      break;
    case 3:
      // dark green
      rgb[0]=0; rgb[1]=160; rgb[2]=0;
      break;
    case 4:
      // pure blue
      rgb[0]=0; rgb[1]=0; rgb[2]=255;
      break;
    case 5:
      // dark blue
      rgb[0]=0; rgb[1]=0; rgb[2]=160;
      break;
    case 6:
      // pure yellow
      rgb[0]=255; rgb[1]=255; rgb[2]=0;
      break;
    case 7:
      // dark yellow
      rgb[0]=160; rgb[1]=160; rgb[2]=0;
      break;
    case 8:
      // background color of choice (default: dark gray)
      rgb[0]=75; rgb[1]=75; rgb[2]=75;
      break;
    default:
      // defaults to white
      rgb[0]=255; rgb[1]=255; rgb[2]=255;
      break;
  }
  glColor3ub(rgb[0], rgb[1], rgb[2]);
  glBegin(GL_POINTS);
  glVertex2i(x*PIXEL_SCALE+2, y*PIXEL_SCALE+2);
  glEnd();
}

// draws singular, flat color to the screen
void clearBackground() 
{
  int x,y;
  for(y=0;y<SCREEN_HEIGHT;y++)
  { 
    for(x=0;x<SCREEN_WIDTH;x++)
    { 
      drawPixel(x,y,8);
    } 
  }	
}

// Prints out the last recorded average FPS to the terminal
// NOTICE: this function should only ever be invoked at the end of displayFrame() 
void printFPS()
{
  displayFrame_Counter++;
  // DEV NOTE: refactor to use short instead of int for memory economy
  int bftSample[fpsSampleRate]; // sampled buffer times (frame1-frame2)
  // init all values to -1; will indicate unset value since FPS samples
  // are always positive
  memset(bftSample, -1, sizeof(bftSample));
  // Pseudo-index of current sampling interval (i.e. for 100 samples, will increment to 100)
  // Compared with nextSampleIdx, which is a concrete, controlled index value for iterating
  // over bftSample array 
  int currentSampleInterval = displayFrame_Counter / (fpsDisplayRate/fpsSampleRate); 

  // sample current FPS at regular intervals, placing the sample in the next vacant index
  if(currentSampleInterval > nextSampleIdx && bftSample[nextSampleIdx] == -1)
  {
    bftSample[nextSampleIdx] = 1000/(Bft.frame1-Bft.frame2);
    sampleSum += bftSample[nextSampleIdx];
    nextSampleIdx++;
  }

  if(displayFrame_Counter >= fpsDisplayRate)
  {
    displayFrame_Counter = 0;
    nextSampleIdx = 0;
    // calculate average frame rate
    int avgFPS = sampleSum / fpsSampleRate;
    sampleSum = 0;
    printf("FPS: %d\n", avgFPS); 
  }
}

// DEBUG; Displays a "palette" showing all supported colors and animated frames
int tick = 0;
void drawTest()
{
  int swHalf = SCREEN_WIDTH/2;
  int shHalf = SCREEN_HEIGHT/2;
  int c = 0; // color ID
  for(int y = 0; y < shHalf; y++)
  {
    for(int x = 0; x < swHalf; x++)
    {
      drawPixel(x, y, c); 
      c += 1;
      if(c > 8) { c = 0; }
    }
  }
  //frame rate
  tick += 1; 
  if(tick>20) { tick = 0; } 
  drawPixel(swHalf, shHalf+tick, 6); 
}

// draws a singular wall on the screen. All coordinate values handled
// by this function are in SCREEN SPACE, not 3D space.
void drawWall(int x1, int x2, int by1, int by2)
{
  // "by" is short for bottom y, aka y coords of bottom edge of wall
  int delta_by = by2 - by1;
  // set delta_x to 1 if 0 to prevent dividing by zero
  int delta_x = x2 - x1; if(delta_x == 0) { delta_x = 1; }

  for(int x = x1; x < x2; x++)
  {
    // 0.5 needed for "rounding issues", investigate further.
    // Current conjecture is that this value "nudges" the current coordinates
    // to the correct placement
    int y = delta_by * (x - x1 + 0.5) / delta_x + by1;
    drawPixel(x, y, 6); // currently hardcoded as yellow, refactor to param later
  }
}

// Renders the current view of the 3D environment
void drawView()
{
  // absolute world position of the wall's 4 points; these change as the
  // player moves and rotates
  int wallX[4], wallY[4], wallZ[4];
  float pCos = TrigVals.cos[Player.angle];
  float pSin = TrigVals.sin[Player.angle];
  wallX[0] = 50;
  wallY[0] = 50;
  wallZ[0] = 0;
  wallX[1] = 50;
  wallY[1] = 300;
  wallZ[1] = 0;
  int FOV = 200; // DEV NOTE: investigate what units this value is
  // offset wall points by player location
  int x1 = wallX[0] - Player.x;
  int y1 = wallY[0] - Player.y;
  int x2 = wallX[1] - Player.x;
  int y2 = wallY[1] - Player.y;
  // offset wall points by player rotation
  wallX[0] = (x1 * pCos) - (y1 * pSin);
  wallY[0] = (y1 * pCos) + (x1 * pSin);
  // z coords are unaffected by looking angle
  wallZ[0] = wallZ[0] - Player.z;

  wallX[1] = (x2 * pCos) - (y2 * pSin);
  wallY[1] = (y2 * pCos) + (x2 * pSin);
  // z coords are unaffected by looking angle
  wallZ[1] = wallZ[1] - Player.z;

  // transform 3D wall points into 2D screen positions
  if(wallY[0] == 0) { wallY[0] = 1; }
  if(wallY[1] == 0) { wallY[1] = 1; }
  wallX[0] = (wallX[0] * FOV) / wallY[0] + (SCREEN_WIDTH / 2); 
  wallY[0] = (wallZ[0] * FOV) / wallY[0] + (SCREEN_HEIGHT / 2);
  wallX[1] = (wallX[1] * FOV) / wallY[1] + (SCREEN_WIDTH / 2);
  wallY[1] = (wallZ[1] * FOV) / wallY[1] + (SCREEN_HEIGHT / 2);
  // draw points on screen
  // DEV NOTE: refactor offscreen clipping to separate function
  // if( wallX[0]>0 && wallX[0]<SCREEN_WIDTH && wallY[0]>0 && wallY[0]<SCREEN_HEIGHT) 
  // {
  //   drawPixel(wallX[0], wallY[0], 6); // 6 is the color code for pure yellow
  // }
  // if( wallX[1]>0 && wallX[1]<SCREEN_WIDTH && wallY[1]>0 && wallY[1]<SCREEN_HEIGHT)
  // {
  //   drawPixel(wallX[1], wallY[1], 6); // 6 is the color code for pure yellow
  // }

  drawWall(wallX[0], wallX[1], wallY[0], wallY[1]);
}
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
    //drawTest(); 
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
  /*printf("frame drawn in %dms\n", Bft.frame1-Bft.frame2);*/
  /*printFPS();*/
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
// initializes critical global variables, such as struct TrigVals
void init()
{
  // compute sin/cos values and store in TrigVals
  for (int i = 0; i < 360; i++) 
  {
    TrigVals.sin[i] = sin(i/180.0 * M_PI);
    TrigVals.cos[i] = cos(i/180.0 * M_PI);
  }
  // initialize player character state
  Player.x = 0;
  Player.y = 0;
  Player.z = 0;
  Player.angle = 0;
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
