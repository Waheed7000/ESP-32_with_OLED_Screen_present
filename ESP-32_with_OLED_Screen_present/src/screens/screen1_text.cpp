#include "Screens.h"

static const char* screen1Lines[] = { "Screen 1" };

Screen screen1TextScreen = {
  "screen1_text",     // name
  SCREEN_TYPE_TEXT,   // type
  screen1Lines,        // textLines
  1,                   // textLineCount
  2,                   // textSize
  nullptr,             // frames (unused)
  0,                   // frameCount
  0                    // frameDelayMs
};