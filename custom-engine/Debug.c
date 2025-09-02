#include <stdio.h>
#include <string.h>
//#include "GameStructs.h"
#include "Debug.h"
#include "Render.h"

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

// Prints out the last recorded average FPS to the terminal
void printFPS(bufferTime *bft)
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
    bftSample[nextSampleIdx] = 1000/(&bft->frame1-&bft->frame2);
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
    printf("\t\t\tFPS: %d\n", avgFPS); 
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
