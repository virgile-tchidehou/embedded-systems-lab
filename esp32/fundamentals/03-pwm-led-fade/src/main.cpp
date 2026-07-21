#include <Arduino.h>

// Wiring — see README.md
// NOTE: uses the Arduino-ESP32 core >= 3.0 LEDC API (ledcAttach/ledcWrite by pin).
constexpr uint8_t LED_PIN = 2;

constexpr uint32_t PWM_FREQ_HZ = 5000;   // above the flicker-fusion threshold
constexpr uint8_t PWM_RESOLUTION = 8;    // duty cycle range: 0-255

constexpr unsigned long STEP_INTERVAL_MS = 15;  // controls fade speed
constexpr int FADE_STEP = 5;

int dutyCycle = 0;
int fadeDirection = 1;
unsigned long lastStep = 0;
uint8_t printDivider = 0;

void setup() {
  Serial.begin(115200);
  ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_RESOLUTION);
}

void loop() {
  if (millis() - lastStep < STEP_INTERVAL_MS) {
    return;
  }
  lastStep = millis();

  ledcWrite(LED_PIN, dutyCycle);

  dutyCycle += fadeDirection * FADE_STEP;
  if (dutyCycle <= 0) {
    dutyCycle = 0;
    fadeDirection = 1;
  } else if (dutyCycle >= 255) {
    dutyCycle = 255;
    fadeDirection = -1;
  }

  // Throttle serial output: one line every ~10 steps instead of every step
  if (++printDivider >= 10) {
    printDivider = 0;
    Serial.printf("duty=%3d/255 (%.0f%%)\n", dutyCycle, dutyCycle / 255.0f * 100.0f);
  }
}
