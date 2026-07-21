#include <Arduino.h>

// Wiring — see README.md for the full diagram
constexpr uint8_t LED_PIN = 2;     // onboard LED on most ESP32 DevKits
constexpr uint8_t BUTTON_PIN = 4;  // push button to GND, internal pull-up enabled

constexpr unsigned long DEBOUNCE_MS = 30;

bool ledState = false;
int lastRawReading = HIGH;
int debouncedState = HIGH;
unsigned long lastEdgeTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, ledState);
}

void loop() {
  int rawReading = digitalRead(BUTTON_PIN);

  // Non-blocking debounce: only trust the reading once it has been
  // stable for DEBOUNCE_MS. Avoids delay() so the loop stays responsive.
  if (rawReading != lastRawReading) {
    lastEdgeTime = millis();
  }

  if ((millis() - lastEdgeTime) > DEBOUNCE_MS && rawReading != debouncedState) {
    debouncedState = rawReading;

    // Button wired to GND with INPUT_PULLUP -> pressed reads LOW
    if (debouncedState == LOW) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      Serial.printf("Button pressed -> LED %s\n", ledState ? "ON" : "OFF");
    }
  }

  lastRawReading = rawReading;
}
