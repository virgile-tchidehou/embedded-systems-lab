#include <Arduino.h>

// Wiring — see README.md
constexpr uint8_t POT_PIN = 34;  // ADC1_CH6, input-only pin (safe: doesn't share ADC2 with Wi-Fi)

constexpr uint8_t OVERSAMPLES = 16;         // averaged per reading, reduces ESP32 ADC noise
constexpr float ADC_MAX = 4095.0f;          // 12-bit resolution
constexpr float V_REF = 3.3f;               // full-scale voltage at 11dB attenuation

constexpr unsigned long PRINT_INTERVAL_MS = 200;
unsigned long lastPrint = 0;

uint32_t readAveraged(uint8_t pin, uint8_t samples) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(100);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);              // 0-4095
  analogSetPinAttenuation(POT_PIN, ADC_11db);  // full 0-3.3V input range
}

void loop() {
  if (millis() - lastPrint >= PRINT_INTERVAL_MS) {
    lastPrint = millis();

    uint32_t raw = readAveraged(POT_PIN, OVERSAMPLES);
    float voltage = (raw / ADC_MAX) * V_REF;
    float percent = (raw / ADC_MAX) * 100.0f;

    Serial.printf("raw=%4u  voltage=%.2fV  %.0f%%\n", raw, voltage, percent);
  }
}
