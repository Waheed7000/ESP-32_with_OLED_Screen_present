#ifndef SCREENS_H
#define SCREENS_H

#include <Arduino.h>

// ============================================================================
// SCREEN TYPE
// Add a new value here whenever a genuinely new kind of content is needed
// (e.g. SCREEN_TYPE_CHART). Existing screens and Rendering.cpp for the other
// types are never affected by adding a new one.
// ============================================================================
enum ScreenType {
  SCREEN_TYPE_TEXT,
  SCREEN_TYPE_IMAGE,
  SCREEN_TYPE_GIF
};

// ============================================================================
// SCREEN FRAME
// One bitmap frame. A static IMAGE has exactly one of these.
// A GIF has several, played back at frameDelayMs intervals.
// 'data' must point to a monochrome bitmap in the format expected by
// Adafruit_GFX::drawBitmap() (typically PROGMEM).
// ============================================================================
struct ScreenFrame {
  const uint8_t* data;
  uint16_t width;
  uint16_t height;
};

// ============================================================================
// SCREEN
// The single generic entity every screen file produces exactly one instance
// of. Only the fields relevant to 'type' are populated; the rest stay at
// their zero/NULL default. Rendering.cpp is the ONLY file that interprets
// these fields — screen files just supply data, main.ino just stores and
// navigates Screen instances without knowing what's inside them.
// ============================================================================
struct Screen {
  const char* name;              // for debug/logging only, never shown as-is

  ScreenType type;

  // ---- used when type == SCREEN_TYPE_TEXT ----
  const char** textLines;        // array of lines, top to bottom
  uint8_t      textLineCount;
  uint8_t      textSize;         // Adafruit_GFX text size multiplier

  // ---- used when type == SCREEN_TYPE_IMAGE or SCREEN_TYPE_GIF ----
  const ScreenFrame* frames;     // 1 frame for IMAGE, N frames for GIF
  uint8_t      frameCount;
  uint16_t     frameDelayMs;     // ignored when frameCount == 1
};

#endif // SCREENS_H