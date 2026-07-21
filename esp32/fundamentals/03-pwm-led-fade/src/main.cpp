#include <Arduino.h>

// ============================================================
// 03 — PWM : Fondu de LED (effet "respiration")
// Utilise le périphérique matériel LEDC de l'ESP32 pour générer un
// signal PWM, sans bloquer la boucle (pas de delay()).
// Câblage détaillé dans le README.md de ce dossier.
//
// ⚠️ Utilise l'API LEDC "par broche" (ledcAttach/ledcWrite) du cœur
// Arduino-ESP32 >= 3.0. Avec un cœur plus ancien (2.x), remplacer par
// ledcSetup() + ledcAttachPin() + ledcWrite(canal, ...).
// ============================================================

constexpr uint8_t LED_PIN = 2;

constexpr uint32_t PWM_FREQ_HZ = 5000;  // au-dessus du seuil de scintillement perceptible par l'œil
constexpr uint8_t PWM_RESOLUTION = 8;   // résolution du rapport cyclique : 0 à 255 (2^8 - 1)

constexpr unsigned long STEP_INTERVAL_MS = 15;  // fréquence de mise à jour du fondu
constexpr int FADE_STEP = 5;                    // incrément du rapport cyclique à chaque pas

int dutyCycle = 0;       // rapport cyclique courant (0 = éteint, 255 = pleine puissance)
int fadeDirection = 1;   // +1 = LED qui monte en intensité, -1 = qui descend
unsigned long lastStep = 0;
uint8_t printDivider = 0;  // compteur pour limiter la fréquence d'affichage série

void setup() {
  Serial.begin(115200);

  // Attache le périphérique matériel LEDC à la broche, avec sa fréquence
  // et sa résolution. Le PWM tourne ensuite en autonomie (le CPU n'a
  // qu'à changer le rapport cyclique via ledcWrite()).
  ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_RESOLUTION);
}

void loop() {
  // Boucle non-bloquante : on ne fait rien tant que STEP_INTERVAL_MS
  // ne s'est pas écoulé depuis le dernier pas de fondu.
  if (millis() - lastStep < STEP_INTERVAL_MS) {
    return;
  }
  lastStep = millis();

  ledcWrite(LED_PIN, dutyCycle);  // applique le rapport cyclique courant sur la broche

  // Avance le rapport cyclique dans la direction courante.
  dutyCycle += fadeDirection * FADE_STEP;

  // Aux bornes (0 ou 255), on inverse la direction : c'est ce qui
  // crée l'effet de va-et-vient ("respiration").
  if (dutyCycle <= 0) {
    dutyCycle = 0;
    fadeDirection = 1;
  } else if (dutyCycle >= 255) {
    dutyCycle = 255;
    fadeDirection = -1;
  }

  // On n'affiche qu'une ligne toutes les ~10 étapes (au lieu de chaque
  // étape) pour ne pas saturer le moniteur série.
  if (++printDivider >= 10) {
    printDivider = 0;
    Serial.printf("duty=%3d/255 (%.0f%%)\n", dutyCycle, dutyCycle / 255.0f * 100.0f);
  }
}
