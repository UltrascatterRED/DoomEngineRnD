# Doom-Like Engine Documentation
This document is a comprehensive guide to the features and use of my doom-like engine (real name TBD).

## Contents
* Overview
* Terminology
* Initialization
* Engine Behavior
* Drawing Frames
* Keyboard inputs

# Overview
This engine was created for me (the developer) to learn about low-level game development, and how I could potentially empower myself to make custom engines for my own projects. The major milestone I am currently aiming for is for this engine to be capable of running a Doom-like First-Person Shooter, or at least a lab demo that proves that capability.

The engine makes heavy use of an OpenGL library for C called Glut. Glut handles almost everything, including drawing frames, drawing the display window itself, and listening for input events.


# Terminology
**Scaled Pixels** - Pixels as they appear on the output display. These pixels may be *scaled* to a different size from the physical pixels in the monitor.

**Hardware Pixels** - The physical pixels in the computer's monitor. Distinct from *scaled pixels*, as they obviously cannot change in size.

**Player** - The movable viewport through which the environment is rendered.

**Screen Space** - Refers to the 2D output window (aka screen). Used to contextualize coordinate values.

**Environment Space** - Refers to the 3D enviroment being represented in the engine. Used to contextualize coordinate values.

# Components
## Global Values
These are globally accessible constants that are intended to be altered as needed.

**Screen Width** - The width, in scaled pixels, of the output window.

**Screen Height** - The height, in scaled pixels, of the output window.

**Pixel Scale** - the ratio of scaled pixels' size to hardware pixels' size. Must be a square value (4 by default).

**GL Window Width** - the width, in hardware pixels, of the output window. Required for certain glut calculations.

**GL Window Height** - the height, in hardware pixels, of the output window. Required for certain glut calculations.

**Milliseconds per Frame (MSPF)** - the approximate number of milliseconds the engine will allocate to drawing each frame. Divide 1000 by this value to get the target minimum frame rate.
### Debug
These values only affect debug behaviors.

**fpsDisplayRate** - number of executions of displayFrame() between FPS printouts. (default: 100,000)

**fpsSampleRate** - number of FPS samples (calculated from bufferTime) to average per FPS printout. (default: 100) 

**Input Keys (multiple values)** - the defined keybinds for each input action, given as an unsigned character value.
* Move Forward (default: w)
* Move Backward (default: s)
* Strafe Left (default: a)
* Strafe Right (default: d)
* Look Left (default: ,)
* Look Right (default: .)
* Fly Up (default: e)
* Fly Down (default: q)
## Structs
These represent a variety of complex entities, from tangible game elements to computation-assisting utilities.

**bufferTime** - *(static)* stores frame buffer timestamps in milliseconds. Used to tell the engine when to advance the frame.<br/><p>Frame buffers are a common part of modern graphics rendering convention, and allow 2 frames to be rendered simultaneously (one to display, one in the background). The frame buffers can then be switched when the frame advances, preventing lag or rendering artifacts.</p>

**keyState** - *(static)* stores the state of each of the input keys. Each key has a corresponding integer value: 1 when pressed, 0 when not pressed.

**trigVals** - *(static)* stores pre-calculated sine and cosine values for degree values, 0 through 359. Used to calculate environment elements' offset relative to the player.

**player** - *(static)* stores state information about the player, such as 3D position (x, y, z) and looking angle (degrees).

**wall** - *(WIP)* stores information about a particular wall, including its (x, y) coordinates and color. Currently not in use, as information about the provided test wall is hardcoded in the main rendering function, drawView().

## Functions
**drawPixel(int x, int y, int color)** - given a screen space coordinate (x, y) and a color code, draws a scaled pixel on the output window at the corresponding position.

**clearBackground()** - draws a single color to all pixels in the output window, effectively clearing its contents.

**printFPS()** - *(debug)* prints the current calculated FPS to the terminal. The behavior of this function can be adjusted by changing certain global variables (see Components -> Global Values -> Debug).

# Initialization
On startup, the engine initializes several important global values. These include the following:
* Width of display window (in scaled pixels)
* Height of display window (in scaled pixels)
* The scale of pixels in the window (i.e. each pixel is a 4x4 square of hardware pixels by default).
* The normalized width and height of the display window (in REAL pixels, not scaled)
  * For use with the Glut libraries.
* Milliseconds per frame (mspf)
  * Used internally to enforce a maximum frame rate
* Miscellaneous debug values for internal use
* The keybinds of defined user-camera actions (aka "player" actions)
  * Move forward/back, strafe left/right, look left/right, etc

## Structs Initialized
  * bufferTime
  * keyState
  * trigVals
  * player

# Player movement
When moving the player around the environment, the environment is actually being moved around the player. At present, there is no particular reason for this, other than that my research has led me down this implementation path. I may opt to refactor this at a later date (i.e. to make accommodating multiple cameras easier). The player will always remain at (0,0,0) with a looking angle of 0 degrees (facing positive Y).

For movement keybinds, see Components -> Global Values.

# Drawing Frames
Using Glut's main loop, the engine refreshes the frame every N milliseconds, where N is the value of the globally defined milliseconds per frame. The engine monitors the state of the struct "bufferTime" to determine when to draw the next frame. The frame's pixels are drawn in layers, starting with a flat background color and ending with the "highest" layer, which is drawn op top of everything last. The current intention is to use this "layering" guideline to facilitate basic environment rendering, where walls, floors, ceilings, etc can be drawn back-to-front relative to the user camera.
