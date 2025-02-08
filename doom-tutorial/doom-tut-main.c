//------------------------------------------------------------------------------
//--------------------------Code By: 3DSage-------------------------------------
//----------------Video tutorial on YouTube-3DSage------------------------------
//------------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <GL/glut.h> 

#define res        1                        //0=160x120 1=360x240 4=640x480
#define SW         160*res                  //screen width
#define SH         120*res                  //screen height
#define SW2        (SW/2)                   //half of screen width
#define SH2        (SH/2)                   //half of screen height
#define pixelScale 4/res                    //OpenGL pixel scale
#define GLSW       (SW*pixelScale)          //OpenGL window width
#define GLSH       (SH*pixelScale)          //OpenGL window height
#define PI         3.141593 
//------------------------------------------------------------------------------
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
 if(Keys.sr==1){ Player.x += delta_y; Player.y -= delta_x; }
 if(Keys.sl==1){ Player.x -= delta_y; Player.y += delta_x; }
 //move up, down, look up, look down
 if(Keys.a==1 && Keys.m==1){ Player.l -= 1; }
 if(Keys.d==1 && Keys.m==1){ Player.l += 1; }
 if(Keys.w==1 && Keys.m==1){ Player.z -= 4; }
 if(Keys.s==1 && Keys.m==1){ Player.z += 4; }
}

void clearBackground() 
{int x,y;
 for(y=0;y<SH;y++)
 { 
  for(x=0;x<SW;x++){ pixel(x,y,8);} //clear background color
 }	
}

int tick;
void draw3D()
{
  int worldx[4], worldy[4], worldz[4];
  float COS = M.cos[Player.a];
  float SIN = M.sin[Player.a];

  // offset bottom 2 points by player position
  int x1 = 40 - Player.x, y1 = 10 - Player.y;
  int x2 = 40 - Player.x, y2 = 290 - Player.y;
  
  // position of point 1
  worldx[0] = x1*COS - y1*SIN; 
  worldy[0] = y1*COS + x1*SIN;
  worldz[0] = 0 - Player.z + ((Player.l * worldy[0])/32.0);
  // position of point 2
  worldx[1] = x2*COS - y2*SIN; 
  worldy[1] = y2*COS + x2*SIN;
  worldz[1] = 0 - Player.z + ((Player.l * worldy[1])/32.0);

  // transform worldx and worldy to their screen positions
  int FOV = 200;
  worldx[0] = worldx[0]*FOV / worldy[0]+SW2;
  worldy[0] = worldz[0]*FOV / worldy[0]+SH2;
  worldx[1] = worldx[1]*FOV / worldy[1]+SW2;
  worldy[1] = worldz[1]*FOV / worldy[1]+SH2;

  // skip drawing negative x and y values (won't be on the screen anyway)
  if(worldx[0]>0 && worldx[0]<SW && worldy[0]>0 && worldy[0]<SH){ pixel(worldx[0], worldy[0], 0); }
  if(worldx[1]>0 && worldx[1]<SW && worldy[1]>0 && worldy[1]<SH){ pixel(worldx[1], worldy[1], 0); }
}
void display() 
{
  int x,y;
  if(T.fr1-T.fr2>=50)                        //only draw 20 frames/second
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

void init()
{
  // store sin and cos as degrees from radians
  int x;
  for(int x = 0; x < 360; x++)
  {
    M.sin[x] = sin(x/180.0*M_PI);
    printf("sine of %d is %f\n", (x+1), M.sin[x]);
    M.cos[x] = cos(x/180.0*M_PI);
    printf("cosine of %d is %f\n", (x+1), M.cos[x]);
  }
  //initialize player
  Player.x = 70; Player.y = -110; Player.z = 20; Player.a = 0; Player.l = 0;
}

int main(int argc, char* argv[])
{
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowPosition(GLSW/2,GLSH/2);
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

