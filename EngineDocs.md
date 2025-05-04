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


## Several critical structs are also initialized
  * bufferTime: stores the timestamp of the last drawn frame and the current elapsed time
    * used to tell Glut when to refresh the frame
  * keyState: stores the state of all defined keyboard inputs
    * referenced constantly in the engine to execute defined input actions (i.e. 'A' pressed --> move forward)
  * trigVals: stores pre-calculated sine and cosine values for each degree value from 0 and 360. This is done to avoid constantly re-calculating these values at runtime.
  * player: stores state information about player(s). The primary function is to keep track of the main player character's state, but it can also be used to likewise track the state of non-player cameras.

# Engine Behavior
## Player movement
When moving the player around the environment, the environment is actually being moved around the player. At present, there is no particular reason for this, other than that my research has led me down this implementation path. I may opt to refactor this at a later date (i.e. to make accommodating multiple cameras easier).

# Drawing Frames
Using Glut's main loop, the engine refreshes the frame every N milliseconds, where N is the value of the globally defined milliseconds per frame. The engine monitors the state of the struct "bufferTime" to determine when to draw the next frame. The frame's pixels are drawn in layers, starting with a flat background color and ending with the "highest" layer, which is drawn op top of everything last. The current intention is to use this "layering" guideline to facilitate basic environment rendering, where walls, floors, ceilings, etc can be drawn back-to-front relative to the user camera.
