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
typedef struct
{
  int frame1, frame2;
}bufferTime; bufferTime Bft;

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

// Renders the current view of the 3D environment
void drawEnvironment()
{
  
}

void displayFrame()
{
  // check if it's time to draw next frame
  if(Bft.frame1-Bft.frame2 >= MSPF)
  {
    clearBackground();
    drawTest(); 
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
  printFPS();
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
  /*init();*/
  glutDisplayFunc(displayFrame);
  /*glutKeyboardFunc(KeysDown);*/
  /*glutKeyboardUpFunc(KeysUp);*/
  glutMainLoop();
  return 0;
}
