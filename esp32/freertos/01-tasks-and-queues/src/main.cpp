#include <Arduino.h>

// ============================================================
// 01 — FreeRTOS : Tâches & Queue
//
// Deux tâches communiquent via une queue FreeRTOS :
//   - task_producer : lit un capteur simulé (potentiomètre) et
//                     envoie la mesure dans la queue toutes les 500ms.
//   - task_consumer : reçoit les messages de la queue, les affiche
//                     sur le moniteur série et pilote une LED en
//                     fonction du seuil.
//
// La LED intégrée s'allume dès que la valeur dépasse SEUIL_HAUT.
//
// Câblage détaillé dans le README.md de ce dossier.
// ============================================================

// ─── Broches ────────────────────────────────────────────────
constexpr uint8_t  LED_PIN     = 2;   // LED intégrée (la plupart des DevKit)
constexpr uint8_t  SENSOR_PIN  = 34;  // ADC1_CH6 — potentiomètre ou analogique libre

// ─── Paramètres ─────────────────────────────────────────────
constexpr uint16_t SEUIL_HAUT  = 2048;  // ~50% de la plage ADC 12 bits
constexpr uint8_t  QUEUE_LEN   = 5;     // nb. de messages max en attente dans la queue

// ─── Structure de message ───────────────────────────────────
// Passer une struct dans la queue (plutôt qu'un simple int) est la bonne
// pratique : ça permet d'ajouter d'autres champs sans changer l'interface.
struct SensorMessage {
  uint16_t raw;       // valeur brute ADC (0–4095)
  float    voltage;   // tension calculée (V)
  uint32_t timestamp; // millis() au moment de la mesure
};

// ─── Handle de la queue ─────────────────────────────────────
// Le handle est le seul lien entre les deux tâches : il n'y a
// aucune variable globale partagée entre elles.
static QueueHandle_t sensorQueue = nullptr;

// ============================================================
// task_producer — tourne sur le Core 1, priorité 2
// Lit le capteur et envoie les données dans la queue.
// ============================================================
void task_producer(void *pv) {
  analogReadResolution(12);
  analogSetPinAttenuation(SENSOR_PIN, ADC_11db);

  for (;;) {
    // Lecture + oversampling minimal (4 échantillons)
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 4; i++) {
      sum += analogRead(SENSOR_PIN);
      delayMicroseconds(100);
    }
    uint16_t raw = sum / 4;

    SensorMessage msg = {
      .raw       = raw,
      .voltage   = (raw / 4095.0f) * 3.3f,
      .timestamp = (uint32_t)millis()
    };

    // xQueueSend est non-bloquant ici (timeout 0) :
    // si la queue est pleine, on abandonne ce message plutôt que de
    // bloquer le producteur. Une queue pleine indique un consumer trop lent.
    if (xQueueSend(sensorQueue, &msg, 0) != pdTRUE) {
      Serial.println("[PROD] ⚠️  Queue pleine — message abandonné !");
    }

    // Délai FreeRTOS non-bloquant : la tâche est suspendue 500 ms,
    // le scheduler peut exécuter d'autres tâches pendant ce temps.
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// ============================================================
// task_consumer — tourne sur le Core 1, priorité 1
// Attend un message de la queue et réagit.
// ============================================================
void task_consumer(void *pv) {
  SensorMessage msg;

  for (;;) {
    // Attente bloquante : la tâche est suspendue jusqu'à la réception
    // d'un message (ou portMAX_DELAY → attente infinie).
    // C'est très différent d'un polling actif : la tâche ne consomme
    // aucun CPU pendant qu'elle attend.
    if (xQueueReceive(sensorQueue, &msg, portMAX_DELAY) == pdTRUE) {

      bool alarme = (msg.raw > SEUIL_HAUT);
      digitalWrite(LED_PIN, alarme ? HIGH : LOW);

      Serial.printf(
        "[CONS] t=%6lu ms | raw=%4u | %.2fV | %s\n",
        msg.timestamp,
        msg.raw,
        msg.voltage,
        alarme ? "⚠️  SEUIL DÉPASSÉ" : "OK"
      );
    }
  }
}

// ============================================================
// setup / loop
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Créer la queue AVANT les tâches qui l'utilisent.
  // sizeof(SensorMessage) : FreeRTOS copie la struct par valeur dans la queue
  // → pas de problème de cycle de vie de la mémoire.
  sensorQueue = xQueueCreate(QUEUE_LEN, sizeof(SensorMessage));
  if (sensorQueue == nullptr) {
    Serial.println("ERREUR : impossible de créer la queue !");
    while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
  }

  // Création des tâches :
  //   xTaskCreatePinnedToCore(fonction, nom, stackSize, param, priorité, handle, core)
  //   - Priorité PLUS HAUTE = le scheduler préempte les tâches moins prioritaires.
  //   - Les deux tâches sont sur le Core 1 : le Core 0 est réservé au Wi-Fi/BT.
  xTaskCreatePinnedToCore(task_producer, "prod", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(task_consumer, "cons", 4096, nullptr, 1, nullptr, 1);

  Serial.println("=== 01 — FreeRTOS : Tasks & Queue ===");
  Serial.printf("Queue créée (longueur %d)\n", QUEUE_LEN);
  Serial.printf("Seuil d'alarme : %d / 4095 (%.1fV)\n", SEUIL_HAUT, (SEUIL_HAUT / 4095.0f) * 3.3f);
  Serial.println("Tournez le potentiomètre pour dépasser le seuil.\n");
}

void loop() {
  // loop() tourne sur le Core 1 à priorité 1 (identique à app_main).
  // En cédant le CPU avec un long délai, on laisse les vraies tâches
  // FreeRTOS s'exécuter sans interférence.
  vTaskDelay(pdMS_TO_TICKS(10000));
}
