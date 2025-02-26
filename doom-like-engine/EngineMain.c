//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\
\\        DOOM-like Game Engine       //
//        by UltrascatterRED          \\
\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//       

// Requisite libraries
#include <stdio.h>
#include <math.h>
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
}

// draws singular, flat color to the screen
void clearBackground() 
{
  int x,y;
  for(y=0;y<SCREEN_HEIGHT;y++)
  { 
    for(x=0;x<SCREEN_WIDTH;x++)
    { 
      drawPixel(x,y,6);
    } 
  }	
}

void displayFrame()
{
  // check if it's time to draw next frame
  if(Bft.frame1-Bft.frame2 >= MSPF)
  {
    clearBackground();
    
    // frame2 holds elapsed time (ms) at which last frame was drawn;
    // frame2 is continuously used to calculate when to draw next frame
    Bft.frame2 = Bft.frame1;
    glutSwapBuffers();
    glutReshapeWindow(GL_WIN_WIDTH, GL_WIN_HEIGHT); // prevents window scaling
  }
  // time elapsed in milliseconds since engine started
  Bft.frame1 = glutGet(GLUT_ELAPSED_TIME);
  glutPostRedisplay();
  printf("frame drawn in %dms\n", Bft.frame1-Bft.frame2); // debug
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
