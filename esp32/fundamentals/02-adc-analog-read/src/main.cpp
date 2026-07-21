#include <Arduino.h>

// ============================================================
// 02 — ADC : Lecture analogique
// Lecture d'un potentiomètre avec sur-échantillonnage (oversampling)
// pour lisser le bruit propre à l'ADC de l'ESP32.
// Câblage détaillé dans le README.md de ce dossier.
// ============================================================

// GPIO34 = broche ADC1_CH6, "input-only" : elle ne peut pas être utilisée
// en sortie, mais elle ne partage pas le convertisseur avec le Wi-Fi
// (contrairement à certaines broches ADC2) — un bon choix par défaut.
constexpr uint8_t POT_PIN = 34;

// Nombre de lectures moyennées pour obtenir une valeur stable.
// Plus ce nombre est grand, plus le bruit est réduit, mais plus la
// mesure devient lente à réagir à un vrai changement.
constexpr uint8_t OVERSAMPLES = 16;

constexpr float ADC_MAX = 4095.0f;  // valeur max sur 12 bits (2^12 - 1)
constexpr float V_REF = 3.3f;       // tension pleine échelle à l'atténuation 11dB

constexpr unsigned long PRINT_INTERVAL_MS = 200;  // limite la fréquence d'affichage série
unsigned long lastPrint = 0;

// Effectue `samples` lectures successives et renvoie leur moyenne.
// C'est la technique la plus simple d'oversampling : elle réduit le
// bruit aléatoire de mesure sans matériel supplémentaire.
uint32_t readAveraged(uint8_t pin, uint8_t samples) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(100);  // petite pause entre échantillons
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);  // résolution de conversion : 0 à 4095

  // Fixe explicitement la plage d'entrée acceptée (0-3.3V) plutôt que
  // de compter sur une valeur par défaut potentiellement différente
  // selon la version du cœur Arduino-ESP32.
  analogSetPinAttenuation(POT_PIN, ADC_11db);
}

void loop() {
  if (millis() - lastPrint >= PRINT_INTERVAL_MS) {
    lastPrint = millis();

    uint32_t raw = readAveraged(POT_PIN, OVERSAMPLES);

    // Conversion de la valeur brute (0-4095) vers une tension réelle,
    // puis vers un pourcentage plus lisible.
    float voltage = (raw / ADC_MAX) * V_REF;
    float percent = (raw / ADC_MAX) * 100.0f;

    Serial.printf("raw=%4u  voltage=%.2fV  %.0f%%\n", raw, voltage, percent);
  }
}
