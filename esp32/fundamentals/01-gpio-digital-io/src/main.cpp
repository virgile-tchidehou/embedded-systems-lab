#include <Arduino.h>

// ============================================================
// 01 — GPIO : Entrée/Sortie digitale
// LED (sortie) + bouton-poussoir (entrée) avec anti-rebond logiciel
// Câblage détaillé dans le README.md de ce dossier.
// ============================================================

// --- Broches ---
constexpr uint8_t LED_PIN = 2;     // LED intégrée sur la plupart des ESP32 DevKit
constexpr uint8_t BUTTON_PIN = 4;  // bouton câblé entre GPIO4 et GND

// --- Anti-rebond ---
// Un bouton mécanique génère plusieurs transitions HIGH/LOW parasites
// (le "rebond") pendant quelques millisecondes à chaque appui.
// On n'accepte une nouvelle valeur que si elle est restée stable
// pendant DEBOUNCE_MS.
constexpr unsigned long DEBOUNCE_MS = 30;

// --- État ---
bool ledState = false;          // état mémorisé de la LED (allumée/éteinte)
int lastRawReading = HIGH;      // dernière lecture brute du bouton (avant filtrage)
int debouncedState = HIGH;      // état filtré, considéré comme fiable
unsigned long lastEdgeTime = 0; // instant du dernier changement de lecture brute

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // INPUT_PULLUP active la résistance de tirage interne : la broche est
  // au niveau HAUT par défaut, et retombe à LOW quand le bouton (câblé
  // vers GND) est pressé. Pas besoin de résistance externe.
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, ledState);
}

void loop() {
  int rawReading = digitalRead(BUTTON_PIN);

  // Dès que la lecture brute change, on redémarre le chronomètre :
  // c'est peut-être un rebond, on attend qu'elle se stabilise.
  if (rawReading != lastRawReading) {
    lastEdgeTime = millis();
  }

  // La lecture est restée stable plus longtemps que DEBOUNCE_MS ET
  // elle diffère de l'état déjà validé : on peut la considérer comme réelle.
  if ((millis() - lastEdgeTime) > DEBOUNCE_MS && rawReading != debouncedState) {
    debouncedState = rawReading;

    // Bouton câblé vers GND avec INPUT_PULLUP => "pressé" se lit LOW.
    if (debouncedState == LOW) {
      ledState = !ledState;                 // on inverse l'état mémorisé
      digitalWrite(LED_PIN, ledState);       // et on l'applique sur la broche
      Serial.printf("Bouton pressé -> LED %s\n", ledState ? "ON" : "OFF");
    }
  }

  lastRawReading = rawReading;

  // Pas de delay() ici : la boucle reste non-bloquante et pourrait
  // gérer d'autres tâches (capteurs, communication...) en parallèle.
}
