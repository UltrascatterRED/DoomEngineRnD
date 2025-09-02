#ifndef RENDER_H
#define RENDER_H

#include "GameStructs.h"
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

void drawPixel(int x, int y, int color);
void clearBackground();
void clipBehindCamera(int *x1, int *y1, int *z1, int x2, int y2, int z2);
void drawWall(int x1, int x2, int by1, int by2, int ty1, int ty2, int color);
void drawView();

#endif
