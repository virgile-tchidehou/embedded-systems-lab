#include <Arduino.h>

// ============================================================
// 04 — FreeRTOS : Software Timers (Timers logiciels)
//
// Les timers logiciels permettent d'exécuter des callbacks
// à intervalles réguliers (Auto-reload) ou différés (One-shot)
// SANS créer de tâches supplémentaires ni consommer de la stack RAM !
//
// Dans ce projet :
// 1. timerClignotant (Auto-reload, 500ms) : fait clignoter la LED
// 2. timerSecurite   (One-shot, 3000ms)    : compte 3s après un appui
//    sur le bouton BOOT (GPIO 0) pour réinitialiser une alerte.
// ============================================================

// ─── Broches ────────────────────────────────────────────────
constexpr uint8_t LED_PIN    = 2;  // LED intégrée
constexpr uint8_t BOUTON_PIN = 0;  // Bouton BOOT (GPIO 0)

// ─── Handles des Timers Logiciels ───────────────────────────
static TimerHandle_t timerClignotant = nullptr; // Timer répétitif (Périodique)
static TimerHandle_t timerSecurite   = nullptr; // Timer à déclenchement unique (One-shot)

// ─── Variable d'état ────────────────────────────────────────
static bool ledEtat = false;

// ============================================================
// Callback 1 : Timer Périodique (Clignotement de la LED)
// ATTENTION : Une callback de timer ne doit JAMAIS bloquer
// (pas de vTaskDelay, pas de boucle infinie) car elle tourne
// dans la tâche système Daemon de FreeRTOS.
// ============================================================
void callbackTimerClignotant(TimerHandle_t xTimer) {
  ledEtat = !ledEtat;
  digitalWrite(LED_PIN, ledEtat ? HIGH : LOW);
  
  Serial.printf("[TIMER PERIODIQUE] ⏱️ Tick 500ms — LED %s\n", ledEtat ? "ON 💡" : "OFF 🔌");
}

// ============================================================
// Callback 2 : Timer Monostable (One-shot Sécurité)
// S'exécute UNE SEULE FOIS 3 secondes après l'appui sur le bouton
// ============================================================
void callbackTimerSecurite(TimerHandle_t xTimer) {
  Serial.println("\n[TIMER ONE-SHOT] 🚨 3 secondes écoulées — Fin de l'alerte de sécurité !");
  Serial.println("[TIMER ONE-SHOT] 🟢 Le système repasse en mode normal.\n");
}

// ============================================================
// Routine d'Interruption (ISR) du Bouton
// Relance le Timer One-Shot de 3 secondes
// ============================================================
void IRAM_ATTR bouton_ISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Réinitialiser / démarrer le timer one-shot depuis l'ISR
  xTimerResetFromISR(timerSecurite, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ============================================================
// Setup / Loop
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BOUTON_PIN, INPUT_PULLUP);
  digitalWrite(LED_PIN, LOW);

  // 1. Création du Timer Périodique (Auto-reload = pdTRUE)
  // Params : (nom, période en ticks, autoReload, id, callback)
  timerClignotant = xTimerCreate(
    "Clignotant",
    pdMS_TO_TICKS(500), // Toutes les 500ms
    pdTRUE,             // Auto-reload (se répète automatiquement)
    nullptr,            // ID (optionnel)
    callbackTimerClignotant
  );

  // 2. Création du Timer One-shot (Auto-reload = pdFALSE)
  timerSecurite = xTimerCreate(
    "Securite",
    pdMS_TO_TICKS(3000), // 3 secondes
    pdFALSE,            // One-shot (s'exécute 1 fois puis s'arrête)
    nullptr,
    callbackTimerSecurite
  );

  if (timerClignotant == nullptr || timerSecurite == nullptr) {
    Serial.println("ERREUR : Impossible de créer les timers logiciels !");
    while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
  }

  // 3. Attacher l'interruption au bouton BOOT
  attachInterrupt(digitalPinToInterrupt(BOUTON_PIN), bouton_ISR, FALLING);

  Serial.println("=== 04 — FreeRTOS : Software Timers ===");
  Serial.println("-> Timer Clignotant (Périodique 500ms) démarré.");
  Serial.println("-> Appuyez sur le bouton BOOT (GPIO 0) pour déclencher le Timer One-Shot (3s).\n");

  // Démarrer le timer périodique immédiatement
  xTimerStart(timerClignotant, 0);
}

void loop() {
  // Pas besoin de créer de tâches supplémentaires !
  // Les timers FreeRTOS sont gérés par la tâche système Timer Daemon.
  vTaskDelay(pdMS_TO_TICKS(10000));
}
