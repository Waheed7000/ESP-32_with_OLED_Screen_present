# ESP-32 with OLED Screen Present

This project runs on an ESP32. It plays a song on a passive buzzer in the
background, and shows a set of "screens" on a 1.3" OLED (SH1106, 128x64,
I2C). Two push buttons let you move forward and backward through the
screens.

This README explains how the project is structured and, more importantly,
how to add new content **without touching the core code**.

## Hardware

- OLED (SH1106, I2C): SDA -> GPIO22, SCL -> GPIO21, address 0x3C
- Passive buzzer: GPIO32
- Forward button: GPIO23
- Backward button: GPIO15
- Both buttons wired as: ESP32 pin -> button -> 10K resistor -> GND
  (software uses `INPUT_PULLUP`, so idle state is HIGH and a press reads LOW)

## File structure

```
ESP-32_with_OLED_Screen_present/
├── ESP-32_with_OLED_Screen_present.ino   (main / orchestrator)
├── Buzzer.h
├── Buzzer.cpp
├── Screens.h
├── Rendering.h
├── Rendering.cpp
└── src/
    └── screens/
        ├── screen1_text.cpp
        └── screen2_rain.cpp
```

Important build detail: the Arduino IDE only compiles extra `.cpp` files
that live directly in the sketch folder, or recursively inside a folder
named `src`. That is why the screen files live under `src/screens/` and
not in a plain `screens/` folder at the root — a plain subfolder like
that would not get compiled at all.

## What each file is responsible for

- **Buzzer.h / Buzzer.cpp** — everything related to the buzzer. The song
  that plays is defined as two arrays (`currentSongNotes` and
  `currentSongDurations`) inside `Buzzer.cpp`. The song plays in the
  background on its own FreeRTOS task (started from `setup()` in main),
  so it never blocks the buttons or the screen drawing.

- **Screens.h** — the data model only. It defines what a "Screen" is
  (a `Screen` struct: text lines, or a set of bitmap frames for an image
  or GIF) and the `ScreenType` enum. It has no actual content in it.

- **src/screens/*.cpp** — one file per screen. Each file only defines one
  global `Screen` variable with that screen's actual data (text, or
  bitmap frames). A screen file never touches drawing code, and it never
  needs to know how it gets rendered.

- **Rendering.h / Rendering.cpp** — the shared drawing logic. This is the
  only place that knows how to actually draw a `Screen` on the OLED,
  based on its `type` (text / image / gif). This logic is written once
  and used for every screen, so it never needs to be duplicated per
  screen file.

- **main.ino** — the orchestrator. It sets up the display, the buzzer
  task and the buttons, keeps an array of pointers to all the `Screen`
  objects, and moves an index forward/backward through that array based
  on button presses. It does not know or care what is inside any screen.

## Button behavior

- Forward button: moves to the next screen. If you are on the last
  screen, it wraps back around to index 0.
- Backward button: moves to the previous screen. If you are already at
  index 0, it stays at 0 (it does not wrap around).

## The infrastructure is not meant to change

The whole point of this structure is that `Buzzer.h/.cpp`,
`Screens.h`, and `Rendering.h/.cpp` are considered done. Nobody should
need to edit them again just to add new content. If you find yourself
about to edit one of those files to add a screen or change the song,
that's a sign something should be done differently instead.

## How to add a new screen

1. Create a new `.cpp` file inside `src/screens/` (name it after its
   content, e.g. `dog_gif.cpp`, not `screen3.cpp`).
2. Inside that file, `#include "Screens.h"` and define one global
   `Screen` variable with your content (text lines, or bitmap frames).
   Look at `screen1_text.cpp` (text example) or `screen2_rain.cpp`
   (animation example) as a template.
3. In `main.ino`, add one `extern Screen yourScreenName;` line next to
   the existing ones.
4. In `main.ino`, add `&yourScreenName` to the `screens[]` array.

That's it. No other file changes.

## How to change the song

Open `Buzzer.cpp` and edit the two arrays `currentSongNotes` and
`currentSongDurations` (they must stay the same length, note-for-note
matched with its duration). Nothing else needs to change — the buzzer
task in `main.ino` just keeps calling `Buzzer_update()`, which always
plays whatever is currently defined in those two arrays.
