//------------------------------------------------------------------------------
//--------------------------Code By: 3DSage-------------------------------------
//----------------Video tutorial on YouTube-3DSage------------------------------
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <GL/glut.h> 

#define res        1                        //0=160x120 1=360x240 4=640x480
//#define SW         160*res                  //screen width
//#define SH         120*res                  //screen height
// NOTE: Calculations in this code are designed for a 4:3 aspect ratio
#define SW         200*res                  //screen width
#define SH         150*res                  //screen height
#define SW2        (SW/2)                   //half of screen width
#define SH2        (SH/2)                   //half of screen height
#define pixelScale 4/res                    //OpenGL pixel scale
#define GLSW       (SW*pixelScale)          //OpenGL window width
#define GLSH       (SH*pixelScale)          //OpenGL window height
#define MSPF       50                       // milliseconds per frame
#define numSects   4                        // total number of sectors
#define numWalls   16                       // total number of walls
//------------------------------------------------------------------------------
// DEBUG: tracks number of executions of display(). 
// Used to print FPS at regular intervals.
int displayfnCount = 0; 

typedef struct 
{
 int fr1,fr2;           //frame 1 frame 2, to create constant frame rate
}time; time T;

typedef struct 
{
 int w,s,a,d;           //move up, down, left, right
 int sl,sr;             //strafe left, right 
 int m;                 //move up, down, look up, down
}keys; keys Keys;

typedef struct 
{
  float sin[360];
  float cos[360];
}math; math M;

typedef struct 
{
  int x, y, z; // player position, z is up
  int a; // angle of rotation
  int l; // represents looking up or down
}player; player Player;

typedef struct 
{
  int x1,y1; // bottom line point 1
  int x2,y2; // bottom line point 2
  int color; // wall color
}walls; walls Walls[30];

typedef struct 
{
  int wall_start, wall_end; // array indices of starting and ending walls in sector
  int z1, z2; // z location of bottom and top of sector respectively
  //int x, y;   // center position of sector
  int distance; // used to draw sectors in order; not sure wtf its a distance *of*
}sectors; sectors Sectors[30];
//------------------------------------------------------------------------------

void pixel(int x,int y, int c)                  //draw a pixel at x/y with rgb
{int rgb[3];
 if(c==0){ rgb[0]=255; rgb[1]=255; rgb[2]=  0;} //Yellow	
 if(c==1){ rgb[0]=160; rgb[1]=160; rgb[2]=  0;} //Yellow darker	
 if(c==2){ rgb[0]=  0; rgb[1]=255; rgb[2]=  0;} //Green	
 if(c==3){ rgb[0]=  0; rgb[1]=160; rgb[2]=  0;} //Green darker	
 if(c==4){ rgb[0]=  0; rgb[1]=255; rgb[2]=255;} //Cyan	
 if(c==5){ rgb[0]=  0; rgb[1]=160; rgb[2]=160;} //Cyan darker
 if(c==6){ rgb[0]=160; rgb[1]=100; rgb[2]=  0;} //brown	
 if(c==7){ rgb[0]=110; rgb[1]= 50; rgb[2]=  0;} //brown darker
 if(c==8){ rgb[0]=  0; rgb[1]= 60; rgb[2]=130;} //background 
 glColor3ub(rgb[0],rgb[1],rgb[2]); 
 glBegin(GL_POINTS);
 glVertex2i(x*pixelScale+2,y*pixelScale+2);
 glEnd();
}

void movePlayer()
{
 //move up, down, left, right
 if(Keys.a ==1 && Keys.m==0){ Player.a -= 4; if(Player.a < 0) { Player.a += 360; } }  
 if(Keys.d ==1 && Keys.m==0){ Player.a += 4; if(Player.a > 359) { Player.a -= 360; } }
 int delta_x = M.sin[Player.a] * 10;
 int delta_y = M.cos[Player.a] * 10;
 if(Keys.w ==1 && Keys.m==0){ Player.x += delta_x; Player.y += delta_y; }
 if(Keys.s ==1 && Keys.m==0){ Player.x -= delta_x; Player.y -= delta_y; }
 //strafe left, right
 if(Keys.sl==1){ Player.x += delta_y; Player.y -= delta_x; }
 if(Keys.sr==1){ Player.x -= delta_y; Player.y += delta_x; }
 //move up, down, look up, look down
 if(Keys.a==1 && Keys.m==1){ Player.l -= 1; }
 if(Keys.d==1 && Keys.m==1){ Player.l += 1; }
 if(Keys.w==1 && Keys.m==1){ Player.z -= 4; }
 if(Keys.s==1 && Keys.m==1){ Player.z += 4; }
}

void clearBackground() 
{
  int x,y;
  for(y=0;y<SH;y++)
  { 
    for(x=0;x<SW;x++){ pixel(x,y,8);} //clear background color
  }	
}

void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2) // clipping line
{
  float distplanepoint_a = *y1; // distance plane, point a
  float distplanepoint_b = y2;  // distance plane, point b
  float distplane = distplanepoint_a - distplanepoint_b; 
  if(distplane == 0){ distplane = 1; } // prevent divide by zero
  float intersection_factor = distplanepoint_a / (distplanepoint_a-distplanepoint_b);
  *x1 = *x1 + intersection_factor*(x2-(*x1));
  *y1 = *y1 + intersection_factor*(y2-(*y1)); 
  if(*y1 == 0){ *y1 = 1; } // prevent divide by zero
  *z1 = *z1 + intersection_factor*(z2-(*z1));
}

// x1, x2: the x coordinates defining the wall's four points.
//  In a vertical wall, two vertically aligned points will
//  have identical x coords.
// by1, by2: the y values of the bottom two points.
void drawWall(int x1, int x2, int by1, int by2, int ty1, int ty2, int color)
{
  int x, y;
  int delta_yb = by2 - by1;
  int delta_yt = ty2 - ty1;
  int delta_x = x2 - x1; if(delta_x == 0) { delta_x = 1; } // set to 1 if 0 to prevent divide by zero error
  int start_x = x1; // begin drawing wall from x1
  
  // Clip offscreen x values (skip drawing them)
  if(x1 < 1) { x1 = 1; } // clip offscreen left
  if(x2 < 1) { x2 = 1; } // clip offscreen left
  if( x1 > SW-1 ) { x1 = SW-1; } // clip offscreen right
  if( x2 > SW-1 ) { x2 = SW-1; } // clip offscreen right

  
  // draw x vertical lines to render wall
  for(x = x1; x < x2; x++)
  {
    int y1 = delta_yb * (x-start_x+0.5)/delta_x+by1; // calculate y coord of point to plot
    int y2 = delta_yt * (x-start_x+0.5)/delta_x+ty1; // calculate y coord of point to plot
  // Clip offscreen y values (skip drawing them)
  if(y1 < 1) { y1 = 1; } // clip offscreen left
  if(y2 < 1) { y2 = 1; } // clip offscreen left
  if( y1 > SH-1 ) { y1 = SH-1; } // clip offscreen right
  if( y2 > SH-1 ) { y2 = SH-1; } // clip offscreen right
    for(y=y1; y<y2;y++)
    {
      pixel(x,y,color);
    }
  }
}
// Calculate distance between two input locations via Pythagorean Theorem
int calculateDistance(int x1, int y1, int x2, int y2)
{
  int distance = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
  return distance;
}

void draw3D()
{
  int s, w, world_x[4], world_y[4], world_z[4];
  
  // bubble sort Sectors array to ensure correct draw order
  for (s = 0; s < numSects - 1; s++) 
  {
    for (w = 0; w < numSects - s - 1; w++) 
    {
      if (Sectors[w].distance < Sectors[w+1].distance) 
      {
        // swap sectors' positions in array
        sectors temp = Sectors[w]; Sectors[w] = Sectors[w+1]; Sectors[w+1] = temp;
      }
    }
  }

  float COS = M.cos[Player.a];
  float SIN = M.sin[Player.a];
  // draw sectors
  for(int s = 0; s < numSects; s++)
  {
    Sectors[s].distance = 0;
    for(int w = Sectors[s].wall_start; w < Sectors[s].wall_end; w++)
    {
      // offset bottom 2 points by player position
      int x1 = Walls[w].x1 - Player.x, y1 = Walls[w].y1 - Player.y;
      int x2 = Walls[w].x2 - Player.x, y2 = Walls[w].y2 - Player.y;
      
      // position of point 1
      world_x[0] = x1*COS - y1*SIN; 
      world_y[0] = y1*COS + x1*SIN;
      world_z[0] = Sectors[s].z1 - Player.z + ((Player.l * world_y[0])/32.0);
      // position of point 2
      world_x[1] = x2*COS - y2*SIN; 
      world_y[1] = y2*COS + x2*SIN;
      world_z[1] = Sectors[s].z1 - Player.z + ((Player.l * world_y[1])/32.0);
      Sectors[s].distance += calculateDistance(0, 0, (world_x[0]+world_x[1])/2, (world_y[0]+world_y[1])/2);
      // position of point 3
      world_x[2] = world_x[0];
      world_y[2] = world_y[0];
      world_z[2] = world_z[0] + Sectors[s].z2;
      // position of point 2
      world_x[3] = world_x[1];
      world_y[3] = world_y[1];
      world_z[3] = world_z[1] + Sectors[s].z2;
      // calculate and store wall distance 
      // don't draw if behind player
      if(world_y[0]<1 && world_y[1]<1){ continue; }
      // clip if point 1 behind player
      if(world_y[0] < 1)
      {
        // bottom line of wall
        clipBehindPlayer(&world_x[0], &world_y[0], &world_z[0], 
                         world_x[1], world_y[1], world_z[1]);
        // top line of wall
        clipBehindPlayer(&world_x[2], &world_y[2], &world_z[2], 
                         world_x[3], world_y[3], world_z[3]);
      }
      // clip if point 2 behind player
      if(world_y[1] < 1)
      {
        // bottom line of wall
        clipBehindPlayer(&world_x[1], &world_y[1], &world_z[1], 
                         world_x[0], world_y[0], world_z[0]);
        // top line of wall
        clipBehindPlayer(&world_x[3], &world_y[3], &world_z[3], 
                         world_x[2], world_y[2], world_z[2]);
      }
    
      // transform world_x and world_y to their screen positions
      // default value: 200
      int FOV = 150;
      world_x[0] = world_x[0]*FOV / world_y[0]+SW2;
      world_y[0] = world_z[0]*FOV / world_y[0]+SH2;
      world_x[1] = world_x[1]*FOV / world_y[1]+SW2;
      world_y[1] = world_z[1]*FOV / world_y[1]+SH2;
      world_x[2] = world_x[2]*FOV / world_y[2]+SW2;
      world_y[2] = world_z[2]*FOV / world_y[2]+SH2;
      world_x[3] = world_x[3]*FOV / world_y[3]+SW2;
      world_y[3] = world_z[3]*FOV / world_y[3]+SH2;
    
      // skip drawing negative x and y values (won't be on the screen anyway)
      /*if(world_x[0]>0 && world_x[0]<SW && world_y[0]>0 && world_y[0]<SH){ pixel(world_x[0], world_y[0], 0); }*/
      /*if(world_x[1]>0 && world_x[1]<SW && world_y[1]>0 && world_y[1]<SH){ pixel(world_x[1], world_y[1], 0); }*/
    
      // draw wall from above coords
      drawWall(world_x[0], world_x[1], world_y[0], world_y[1], world_y[2], world_y[3], Walls[w].color);
    }
    // find average distance of sector
    Sectors[s].distance /= (Sectors[s].wall_end - Sectors[s].wall_start);
  }
}
void display() 
{

  int fpsDisplayRate = 100000;  // how many executions of this function between FPS printout updates
  int x,y;
  if(T.fr1-T.fr2 >= MSPF) 
  { 
    clearBackground();
    movePlayer();
    draw3D();

    T.fr2=T.fr1;   
    glutSwapBuffers(); 
    glutReshapeWindow(GLSW,GLSH);             //prevent window scaling
  }

  T.fr1=glutGet(GLUT_ELAPSED_TIME);          //1000 Milliseconds per second
  glutPostRedisplay();
  /*printf("frame drawn in %dms\n", T.fr1-T.fr2); // debug*/
  displayfnCount++;
  if(displayfnCount >= fpsDisplayRate)
  {
    displayfnCount = 0;
    printf("FPS: %d\n", 1000/(T.fr1-T.fr2)); 
  }
} 

void KeysDown(unsigned char key,int x,int y)   
{ 
 if(key=='w'==1){ Keys.w =1;} 
 if(key=='s'==1){ Keys.s =1;} 
 if(key=='a'==1){ Keys.a =1;} 
 if(key=='d'==1){ Keys.d =1;} 
 if(key=='m'==1){ Keys.m =1;} 
 if(key==','==1){ Keys.sr=1;} 
 if(key=='.'==1){ Keys.sl=1;} 
}
void KeysUp(unsigned char key,int x,int y)
{ 
 if(key=='w'==1){ Keys.w =0;}
 if(key=='s'==1){ Keys.s =0;}
 if(key=='a'==1){ Keys.a =0;}
 if(key=='d'==1){ Keys.d =0;}
 if(key=='m'==1){ Keys.m =0;}
 if(key==','==1){ Keys.sr=0;} 
 if(key=='.'==1){ Keys.sl=0;}
}

int sectorsInit[] = 
{
  // wall_start, wall_end, z1, z2
  0, 4, 0, 40, // sector 1
  4, 8, 0, 40, // sector 2
  8, 12, 0, 40, // sector 3
  12, 16, 0, 40 // sector 4
};

int wallsInit[] = 
{
  // x1, y1, x2, y2, color
  0, 0, 32, 0, 0,
  32, 0, 32, 32, 1,
  32, 32, 0, 32, 0,
  0, 32, 0, 0, 1,

  64, 0, 96, 0, 2,
  96, 0, 96, 32, 3,
  96, 32, 64, 32, 2,
  64, 32, 64, 0, 3,

  64, 64, 96, 64, 4,
  96, 64, 96, 96, 5,
  96, 96, 64, 96, 4,
  64, 96, 64, 64, 5,

  0, 64, 32, 64, 6,
  32, 64, 32, 96, 7,
  32, 96, 0, 96, 6,
  0, 96, 0, 64, 7
};

void init()
{
  // store sin and cos as degrees from radians
  int x;
  for(int x = 0; x < 360; x++)
  {
    M.sin[x] = sin(x/180.0*M_PI);
    //printf("sine of %d is %f\n", (x+1), M.sin[x]);
    M.cos[x] = cos(x/180.0*M_PI);
    //printf("cosine of %d is %f\n", (x+1), M.cos[x]);
  }
  //initialize player
  Player.x = 70; Player.y = -110; Player.z = 20; Player.a = 0; Player.l = 0;
  // load sectors
  int s, w, v1 = 0, v2 = 0; // v1, v2 index rows for sector and wall init arrays respectively
  for (s = 0; s < numSects; s++) 
  {
    Sectors[s].wall_start = sectorsInit[v1+0];
    Sectors[s].wall_end = sectorsInit[v1+1];
    Sectors[s].z1 = sectorsInit[v1+2];
    Sectors[s].z2 = sectorsInit[v1+3];
    v1 += 4;
    for(w = Sectors[s].wall_start; w < Sectors[s].wall_end; w++)
    {
      Walls[w].x1 = wallsInit[v2+0];
      Walls[w].y1 = wallsInit[v2+1];
      Walls[w].x2 = wallsInit[v2+2];
      Walls[w].y2 = wallsInit[v2+3];
      Walls[w].color = wallsInit[v2+4];
      v2 += 5;
    }
  }
}

int main(int argc, char* argv[])
{
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowPosition(0, 0); // defaults: GLSW/2,GLSH/2
 glutInitWindowSize(GLSW,GLSH);
 glutCreateWindow(""); 
 glPointSize(pixelScale);                        //pixel size
 gluOrtho2D(0,GLSW,0,GLSH);                      //origin bottom left
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(KeysDown);
 glutKeyboardUpFunc(KeysUp);
 glutMainLoop();
 return 0;
} 

