#include <Arduino.h>

// ============================================================
// 03 — FreeRTOS : Event Groups (Groupes d'événements)
//
// Un Event Group permet d'attendre PLUSIEURS conditions (bits)
// avant de débloquer une tâche.
//
// Dans cet exemple :
// 3 tâches d'initialisation (tache_wifi, tache_capteurs, tache_moteur)
// effectuent un autotest au démarrage. Chaque tâche active son BIT.
//
// La tache_superviseur attend que TOUS LES BITS soient activés
// (BIT_WIFI | BIT_CAPTEURS | BIT_MOTEUR) avant d'autoriser le
// démarrage du système et d'allumer la LED.
// ============================================================

// ─── Broche ─────────────────────────────────────────────────
constexpr uint8_t LED_PIN = 2; // LED intégrée (DevKit)

// ─── Masques de bits d'événements ───────────────────────────
// Dans FreeRTOS, les 24 premiers bits (0 à 23) d'un EventGroup sont utilisables.
constexpr EventBits_t BIT_WIFI     = (1 << 0); // Bit 0 -> 0b001
constexpr EventBits_t BIT_CAPTEURS = (1 << 1); // Bit 1 -> 0b010
constexpr EventBits_t BIT_MOTEUR   = (1 << 2); // Bit 2 -> 0b100

// Combinaison de tous les bits requis pour démarrer le système
constexpr EventBits_t TOUS_LES_BITS = (BIT_WIFI | BIT_CAPTEURS | BIT_MOTEUR); // 0b111

// ─── Handle de l'Event Group ────────────────────────────────
static EventGroupHandle_t groupeEvenements = nullptr;

// ============================================================
// Tâche 1 : Initialisation du Wi-Fi (prend 2 secondes)
// ============================================================
void tache_wifi(void *pv) {
  Serial.println("[WIFI] 📡 Connexion Wi-Fi en cours...");
  vTaskDelay(pdMS_TO_TICKS(2000)); // Simulation d'une tâche qui prend 2s

  Serial.println("[WIFI] ✅ Wi-Fi connecté avec succès !");
  
  // Activer le bit du Wi-Fi dans le groupe d'événements
  xEventGroupSetBits(groupeEvenements, BIT_WIFI);

  // La tâche se termine une fois l'initialisation faite
  vTaskDelete(nullptr);
}

// ============================================================
// Tâche 2 : Vérification des Capteurs (prend 3.5 secondes)
// ============================================================
void tache_capteurs(void *pv) {
  Serial.println("[CAPTEURS] 🔍 Autotest des capteurs...");
  vTaskDelay(pdMS_TO_TICKS(3500)); // Simulation de 3.5s

  Serial.println("[CAPTEURS] ✅ Capteurs prêts et calibrés !");

  // Activer le bit des capteurs
  xEventGroupSetBits(groupeEvenements, BIT_CAPTEURS);

  vTaskDelete(nullptr);
}

// ============================================================
// Tâche 3 : Vérification du Moteur (prend 1 seconde)
// ============================================================
void tache_moteur(void *pv) {
  Serial.println("[MOTEUR] ⚙️ Test du driver moteur...");
  vTaskDelay(pdMS_TO_TICKS(1000)); // Simulation de 1s

  Serial.println("[MOTEUR] ✅ Moteur opérationnel !");

  // Activer le bit du moteur
  xEventGroupSetBits(groupeEvenements, BIT_MOTEUR);

  vTaskDelete(nullptr);
}

// ============================================================
// Tâche Superviseur : Attente de la barrière d'initialisation
// ============================================================
void tache_superviseur(void *pv) {
  Serial.println("[SUPERVISEUR] ⏳ En attente de l'initialisation complète...");

  // xEventGroupWaitBits (
  //   groupe,            // Le groupe d'événements à observer
  //   bitsToWaitFor,     // Les bits à attendre (BIT_WIFI | BIT_CAPTEURS | BIT_MOTEUR)
  //   clearOnExit,       // pdTRUE = effacer les bits automatiquement quand la tâche est débloquée
  //   waitForAllBits,    // pdTRUE = attendre que TOUS les bits soient mis à 1 (ET logique)
  //   xTicksToWait       // portMAX_DELAY = attente infinie jusqu'à validation
  // )
  EventBits_t bitsObtenus = xEventGroupWaitBits(
    groupeEvenements,
    TOUS_LES_BITS,
    pdTRUE,  // Effacer les bits en sortant
    pdTRUE,  // Attendre TOUS les bits
    portMAX_DELAY
  );

  // Vérifier si tous les bits demandés ont été validés
  if ((bitsObtenus & TOUS_LES_BITS) == TOUS_LES_BITS) {
    Serial.println("\n==============================================");
    Serial.println("🚀 SYSTÈME PRÊT ! Tous les sous-systèmes sont OK.");
    Serial.println("==============================================\n");

    // Allumer la LED pour indiquer que le système est fonctionnel
    digitalWrite(LED_PIN, HIGH);
  }

  for (;;) {
    // La tâche continue d'exécuter la boucle principale du superviseur
    Serial.println("[SUPERVISEUR] 🟢 Système en cours de fonctionnement...");
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// ============================================================
// Setup / Loop
// ============================================================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Création du groupe d'événements
  groupeEvenements = xEventGroupCreate();

  if (groupeEvenements == nullptr) {
    Serial.println("ERREUR : Impossible de créer le groupe d'événements !");
    while (true) { vTaskDelay(pdMS_TO_TICKS(1000)); }
  }

  Serial.println("=== 03 — FreeRTOS : Event Groups ===");
  Serial.println("Démarrage de la séquence d'initialisation multi-tâches...\n");

  // Lancement des 3 tâches d'initialisation en parallèle
  xTaskCreatePinnedToCore(tache_wifi,       "wifi",      2048, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(tache_capteurs,   "capteurs",  2048, nullptr, 1, nullptr, 1);
  xTaskCreatePinnedToCore(tache_moteur,     "moteur",    2048, nullptr, 1, nullptr, 1);

  // Tâche de supervision (priorité 2 pour s'exécuter immédiatement quand les bits sont prêts)
  xTaskCreatePinnedToCore(tache_superviseur, "superv",   2048, nullptr, 2, nullptr, 1);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(10000));
}
