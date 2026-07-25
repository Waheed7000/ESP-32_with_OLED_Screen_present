#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Buzzer.h"
#include "Screens.h"
#include "Rendering.h"

// ============================================================================
// OLED CONFIG
// ============================================================================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_SDA      22
#define OLED_SCL      21
#define OLED_I2C_ADDR 0x3C

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ============================================================================
// BUTTONS CONFIG
// Wiring: ESP_PIN -> Button -> 10K resistor -> GND (INPUT_PULLUP used in sw)
// ============================================================================
#define BUTTON_FORWARD_PIN  23
#define BUTTON_BACKWARD_PIN 15
#define BUTTON_DEBOUNCE_MS  50

static uint8_t  rawForwardState    = HIGH;
static uint8_t  debouncedForward   = HIGH;
static uint32_t lastForwardChangeTime = 0;

static uint8_t  rawBackwardState   = HIGH;
static uint8_t  debouncedBackward  = HIGH;
static uint32_t lastBackwardChangeTime = 0;

void Buttons_init() {
  pinMode(BUTTON_FORWARD_PIN, INPUT_PULLUP);
  pinMode(BUTTON_BACKWARD_PIN, INPUT_PULLUP);
}

bool Buttons_forwardPressed() {
  uint8_t reading = digitalRead(BUTTON_FORWARD_PIN);
  if (reading != rawForwardState) {
    rawForwardState = reading;
    lastForwardChangeTime = millis();
  }
  bool pressed = false;
  if ((millis() - lastForwardChangeTime) > BUTTON_DEBOUNCE_MS) {
    if (reading != debouncedForward) {
      debouncedForward = reading;
      if (debouncedForward == LOW) pressed = true;
    }
  }
  return pressed;
}

bool Buttons_backwardPressed() {
  uint8_t reading = digitalRead(BUTTON_BACKWARD_PIN);
  if (reading != rawBackwardState) {
    rawBackwardState = reading;
    lastBackwardChangeTime = millis();
  }
  bool pressed = false;
  if ((millis() - lastBackwardChangeTime) > BUTTON_DEBOUNCE_MS) {
    if (reading != debouncedBackward) {
      debouncedBackward = reading;
      if (debouncedBackward == LOW) pressed = true;
    }
  }
  return pressed;
}

// ============================================================================
// SCREENS REGISTRY
// This is the ONLY part of main.ino that changes when a screen is added:
// 1) one extern line   2) one entry in the array below.
// ============================================================================
extern Screen screen1TextScreen;
extern Screen screen2RainScreen;

Screen* screens[] = {
  &screen1TextScreen,
  &screen2RainScreen,
};

const uint16_t TOTAL_SCREENS = sizeof(screens) / sizeof(screens[0]);

int16_t currentScreenIndex = 0;
int16_t lastDrawnIndex = -1;

// ============================================================================
// BUZZER TASK - runs independently in the background on Core 0.
// Nothing else in the project needs to know this exists or call it.
// ============================================================================
void BuzzerTask(void* parameter) {
  for (;;) {
    Buzzer_update();
    vTaskDelay(pdMS_TO_TICKS(5)); // small poll interval, keeps timing tight
  }
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(OLED_I2C_ADDR, true)) {
    Serial.println(F("SH1106 OLED not found - check wiring!"));
    while (true) delay(1000);
  }

  Rendering_init(&display);
  Buzzer_init();
  Buttons_init();

  xTaskCreatePinnedToCore(
    BuzzerTask,     // task function
    "BuzzerTask",   // name
    2048,           // stack size
    NULL,           // parameter
    1,              // priority
    NULL,           // task handle
    0               // run on Core 0 (main loop runs on Core 1)
  );

  display.clearDisplay();
  display.display();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  if (Buttons_forwardPressed()) {
    if (TOTAL_SCREENS > 0) {
      currentScreenIndex = (currentScreenIndex + 1) % TOTAL_SCREENS;
    }
  }

  if (Buttons_backwardPressed()) {
    if (currentScreenIndex > 0) {
      currentScreenIndex--;
    }
  }

  if (TOTAL_SCREENS > 0 && currentScreenIndex != lastDrawnIndex) {
    lastDrawnIndex = currentScreenIndex;
    Rendering_draw(screens[currentScreenIndex]);
  }
}