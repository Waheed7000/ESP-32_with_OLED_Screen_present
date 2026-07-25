#include "Buzzer.h"

void Buzzer_init() {
  ledcAttach(BUZZER_PIN, 2000, BUZZER_PWM_RESOLUTION_BITS);
  ledcWriteTone(BUZZER_PIN, 0);
}

void Buzzer_playTone(uint16_t frequency, uint16_t duration) {
  if (frequency == 0) {
    ledcWriteTone(BUZZER_PIN, 0);
    delay(duration);
    return;
  }
  ledcWriteTone(BUZZER_PIN, frequency);
  delay((duration * 9) / 10);
  ledcWriteTone(BUZZER_PIN, 0);
  delay(duration / 10);
}

void Buzzer_playMelody(const uint16_t* notes, const uint16_t* durations, uint16_t noteCount) {
  for (uint16_t i = 0; i < noteCount; i++) {
    Buzzer_playTone(notes[i], durations[i]);
  }
}

// ============================================================================
// CURRENT SONG - change ONLY these two arrays to change what plays.
// Nothing else in the project needs to change.
// ============================================================================
static const uint16_t currentSongNotes[] = {
  523, 440, 494, 523, 440, 494, 523,
  440, 349, 392, 440, 349, 392, 440,
  349, 392, 440, 392, 349,
  294, 294, 294, 294, 262, 294, 262
};

static const uint16_t currentSongDurations[] = {
  500, 250, 250, 500, 250, 250, 1000,
  500, 250, 250, 500, 250, 250, 1000,
  500, 250, 250, 500, 500,
  500, 250, 250, 500, 500, 500, 1000
};

// ============================================================================
// BACKGROUND PLAYBACK STATE - do not touch, this is generic playback logic
// ============================================================================
enum BuzzerPhase { BUZZER_PHASE_TONE, BUZZER_PHASE_GAP };

static uint16_t     currentNoteIndex = 0;
static uint32_t     phaseStartTime   = 0;
static BuzzerPhase  currentPhase     = BUZZER_PHASE_TONE;
static bool         buzzerStarted    = false;

void Buzzer_update() {
  uint16_t noteCount = sizeof(currentSongNotes) / sizeof(currentSongNotes[0]);

  if (!buzzerStarted) {
    buzzerStarted = true;
    phaseStartTime = millis();
    if (currentSongNotes[0] > 0) ledcWriteTone(BUZZER_PIN, currentSongNotes[0]);
  }

  uint16_t duration     = currentSongDurations[currentNoteIndex];
  uint16_t toneDuration = (duration * 9) / 10;
  uint16_t gapDuration  = duration - toneDuration;
  uint32_t elapsed      = millis() - phaseStartTime;

  if (currentPhase == BUZZER_PHASE_TONE && elapsed >= toneDuration) {
    ledcWriteTone(BUZZER_PIN, 0);           // gap for note articulation
    currentPhase = BUZZER_PHASE_GAP;
    phaseStartTime = millis();

  } else if (currentPhase == BUZZER_PHASE_GAP && elapsed >= gapDuration) {
    currentNoteIndex = (currentNoteIndex + 1) % noteCount; // loops forever
    if (currentSongNotes[currentNoteIndex] > 0) {
      ledcWriteTone(BUZZER_PIN, currentSongNotes[currentNoteIndex]);
    }
    currentPhase = BUZZER_PHASE_TONE;
    phaseStartTime = millis();
  }
}