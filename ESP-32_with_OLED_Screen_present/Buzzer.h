#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

#define BUZZER_PIN 32
#define BUZZER_PWM_RESOLUTION_BITS 8

// Call once from setup().
void Buzzer_init();

// Plays a single tone (0 = silence) for duration ms, with a short
// silent gap at the end for note articulation.
void Buzzer_playTone(uint16_t frequency, uint16_t duration);

// Plays a full melody: two parallel arrays (notes[i] with durations[i]),
// noteCount long. Whoever adds a song just fills arrays like this
// elsewhere and calls Buzzer_playMelody(notes, durations, count).
void Buzzer_playMelody(const uint16_t* notes, const uint16_t* durations, uint16_t noteCount);

void Buzzer_update(); // call EVERY loop() iteration - non-blocking, plays currentSong forever in the background
#endif // BUZZER_H