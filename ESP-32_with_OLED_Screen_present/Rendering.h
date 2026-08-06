#ifndef RENDERING_H
#define RENDERING_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "Screens.h"

// Call once from setup() after display.begin(...).
void Rendering_init(Adafruit_SH1106G* displayPtr);

// Draws the given screen fully (clears, draws content, pushes to display).
// For GIF screens this blocks internally, looping frames until 'holdMs'
// has elapsed (so navigation stays responsive-ish); call again if you need
// it to keep animating longer.
void Rendering_draw(const Screen* screen, uint32_t holdMs = 1000);

#endif // RENDERING_H