#include <Arduino.h>

// ============================================================
// 02 — FreeRTOS : Sémaphores & Mutexes
//
// Ce projet illustre 2 concepts fondamentaux de FreeRTOS :
//
// 1. LE MUTEX (xSemaphoreCreateMutex) :
//    Deux tâches (tache_temperature et tache_humidite) veulent écrire
//    sur le port Série (Serial). Le Mutex empêche les deux tâches
//    d'écrire en même temps et de mélanger leur texte.
//
// 2. LE SÉMAPHORE BINAIRE (xSemaphoreCreateBinary) :
//    Quand on appuie sur le bouton (interruption matérielle ISR),
//    l'ISR envoie un sémaphore binaire pour réveiller immédiatement
//    la tache_bouton qui allume/éteint la LED.
// ============================================================

// ─── Broches ────────────────────────────────────────────────
constexpr uint8_t LED_PIN    = 2;  // LED intégrée sur ESP32 DevKit
constexpr uint8_t BOUTON_PIN = 0;  // Bouton BOOT (ou bouton externe sur GPIO 0)

// ─── Handles FreeRTOS ───────────────────────────────────────
// Le Mutex protège l'accès au moniteur série (Serial)
static SemaphoreHandle_t serialMutex = nullptr;

// Le Sémaphore binaire sert de signal entre le bouton (ISR) et la tâche
static SemaphoreHandle_t boutonSemaphore = nullptr;

// ─── Variable d'état simple ────────────────────────────────
static bool ledEtat = false;

// ============================================================
// Routine d'Interruption (ISR) du Bouton
// ATTENTION : Une ISR doit être TRÈS courte !
// Pas de Serial.println(), pas de delay() dans une ISR.
// ============================================================
void IRAM_ATTR bouton_ISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Signaler à la tâche qu'un appui a eu lieu
  xSemaphoreGiveFromISR(boutonSemaphore, &xHigherPriorityTaskWoken);

  // Demander au scheduler d'exécuter la tâche si elle est prioritaire
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ============================================================
// Tâche 1 : Simulation de la mesure de température
// S'exécute toutes les 2 secondes
// ============================================================
void tache_temperature(void *pv) {
  for (;;) {
    float temp = 22.5f + (random(0, 50) / 10.0f); // Valeur simulée entre 22.5°C et 27.5°C

    // Prendre le Mutex avant d'utiliser Serial (attente bloquante si occupé)
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
      Serial.printf("[TEMP] 🌡️  Température mesurée : %.1f °C\n", temp);
      
      // Libérer le Mutex pour les autres tâches
      xSemaphoreGive(serialMutex);
    }

    // Attendre 2000 ms sans bloquer le microcontrôleur
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

// ============================================================
// Tâche 2 : Simulation de la mesure d'humidité
// S'exécute toutes les 3 secondes
// ============================================================
void tache_humidite(void *pv) {
  for (;;) {
    int humidite = 50 + random(0, 30); // Valeur simulée entre 50% et 80%

    // Prendre le Mutex avant d'utiliser Serial
    if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
      Serial.printf("[HUMI] 💧  Humidité mesurée    : %d %%\n", humidite);
      
      // Libérer le Mutex
      xSemaphoreGive(serialMutex);
    }

    // Attendre 3000 ms
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

// ============================================================
// Tâche 3 : Traitement de l'appui sur le bouton
// Attend le sémaphore binaire envoyé par l'ISR
// ============================================================
void tache_bouton(void *pv) {
  for (;;) {
    // Attendre le sémaphore envoyé par l'interruption (bloquant)
    if (xSemaphoreTake(boutonSemaphore, portMAX_DELAY) == pdTRUE) {
      
      // Inverser l'état de la LED
      ledEtat = !ledEtat;
      digitalWrite(LED_PIN, ledEtat ? HIGH : LOW);

      // Afficher un message de manière sécurisée avec le Mutex
      if (xSemaphoreTake(serialMutex, portMAX_DELAY) == pdTRUE) {
        Serial.printf("[BOUTON] 🔘 Appui détecté ! LED %s\n", ledEtat ? "ALLUMÉE 💡" : "ÉTEINTE 🔌");
        xSemaphoreGive(serialMutex);
      }
    }
  }
}

// ============================================================
// Setup / Loop
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BOUTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, LOW);

  // 1. Créer le Mutex pour la protection du Serial
  serialMutex = xSemaphoreCreateMutex();

  // 2. Créer le Sémaphore Binaire pour le bouton
  boutonSemaphore = xSemaphoreCreateBinary();

  if (serialMutex == nullptr || boutonSemaphore == nullptr) {
    Serial.println("ERREUR : Impossible de créer les sémaphores / mutex !");
    while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
  }

  // 3. Attacher l'interruption matérielle sur la broche du bouton (front descendant / FALLING)
  attachInterrupt(digitalPinToInterrupt(BOUTON_PIN), bouton_ISR, FALLING);

  // 4. Créer les tâches FreeRTOS sur le Core 1
  xTaskCreatePinnedToCore(tache_temperature, "temp",   2048, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(tache_humidite,    "humi",   2048, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(tache_bouton,      "bouton", 2048, nullptr, 2, nullptr, 1); // Priorité 2 plus haute pour réagir vite au bouton

  Serial.println("=== 02 — FreeRTOS : Sémaphores & Mutexes ===");
  Serial.println("-> Mutex créé pour sécuriser le moniteur Serie.");
  Serial.println("-> Sémaphore Binaire créé pour gérer le bouton via ISR.");
  Serial.println("Appuyez sur le bouton BOOT (GPIO 0) pour tester l'interruption !\n");
}

void loop() {
  // Laisser le scheduler FreeRTOS gérer les tâches
  vTaskDelay(pdMS_TO_TICKS(10000));
}
