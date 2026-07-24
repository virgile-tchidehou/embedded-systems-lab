# 02 — FreeRTOS : Sémaphores & Mutexes

Trois tâches FreeRTOS et une interruption matérielle (ISR) collaborent en utilisant un **Mutex** (pour protéger le port Série partagé) et un **Sémaphore Binaire** (pour synchroniser l'appui sur un bouton).

---

## 🎯 Quel est l'objectif ?

- Comprendre la différence fondamentale entre un **Mutex** et un **Sémaphore Binaire**
- Proteger une ressource partagée (`Serial`) contre les accès concurrents avec `xSemaphoreCreateMutex`, `xSemaphoreTake` et `xSemaphoreGive`
- Gérer une interruption matérielle (ISR) de façon ultra-rapide en déléguant le travail lourd à une tâche via `xSemaphoreCreateBinary` et `xSemaphoreGiveFromISR`
- Découvrir la notion de réentrabilité et de section critique en embarqué

---

## 💡 Pourquoi cette technologie est-elle importante ?

### 1. Le problème des accès concurrents (Mutex)
Dans un système multi-tâches, si deux tâches essaient d'utiliser en même temps un même périphérique (le bus I2C, un écran LCD, ou la liaison Série `Serial`), les données se mélangent et provoquent un comportement imprévisible.

Le **Mutex** agit comme un "jeton d'accès exclusif" : une seule tâche peut le posséder à la fois. Si une autre tâche le veut, elle attend patiemment que la première le libère.

### 2. Le problème des interruptions (Sémaphore Binaire)
Dans une routine d'interruption (ISR) déclenchée par un composant physique (bouton, capteur de mouvement, signal radio), on ne doit **jamais** exécuter de long traitement ni appeler des fonctions bloquantes comme `Serial.println()` ou `delay()`. Cela ferait planter l'ESP32.

Le **Sémaphore Binaire** permet à l'ISR de faire le strict minimum : donner un signal (`xSemaphoreGiveFromISR`), puis s'arrêter. La tâche réceptrice (`tache_bouton`) se réveille immédiatement pour exécuter le travail en toute sécurité.

---

## 🛠️ Quel matériel est utilisé ?

| Composant | Rôle |
|---|---|
| ESP32 DevKit | Microcontrôleur (double cœur Xtensa LX6) |
| Bouton BOOT (GPIO 0) | Source d'interruption matérielle (signal descendant / FALLING) |
| LED intégrée (GPIO 2) | Acteur physique piloté lors de l'appui sur le bouton |

---

## ⚙️ Comment fonctionne le système ?

```
                  ┌──────────────────────────────┐
                  │      Mutex serialMutex       │
                  └──────────────┬───────────────┘
                                 │  (Accès exclusif à Serial)
            ┌────────────────────┼────────────────────┐
            ▼                                         ▼
┌───────────────────────┐                 ┌───────────────────────┐
│  tache_temperature    │                 │    tache_humidite     │
│  Core 1 | Priorité 1  │                 │  Core 1 | Priorité 1  │
│  (Toutes les 2s)      │                 │  (Toutes les 3s)      │
└───────────────────────┘                 └───────────────────────┘

───────────────────────────────────────────────────────────────────

   ISR Bouton (GPIO 0)                     Tâche Bouton
┌───────────────────────┐  Sémaphore   ┌───────────────────────┐
│     bouton_ISR()      │ ── Binaire ─►│     tache_bouton      │
│ xSemaphoreGiveFromISR │              │  Core 1 | Priorité 2  │
└───────────────────────┘              │  Toggle LED & Log     │
                                       └───────────────────────┘
```

1. **`tache_temperature` & `tache_humidite`** : Demandent le Mutex `serialMutex` avant d'afficher leurs mesures. La première qui prend le Mutex verrouille l'accès.
2. **`bouton_ISR`** : Déclenché instantanément lors de l'appui sur GPIO 0. Elle donne le Sémaphore Binaire et passe la main.
3. **`tache_bouton`** : Suspendue en attente du Sémaphore. Dès réception, elle s'exécute avec la priorité 2 (priorité supérieure aux mesures) et bascule la LED.

---

## 🔁 Comment reproduire l'expérience ?

**Build & flash avec PlatformIO**

```bash
pio run -t upload
pio device monitor
```

**Comportement attendu**

```
=== 02 — FreeRTOS : Sémaphores & Mutexes ===
-> Mutex créé pour sécuriser le moniteur Serie.
-> Sémaphore Binaire créé pour gérer le bouton via ISR.
Appuyez sur le bouton BOOT (GPIO 0) pour tester l'interruption !

[TEMP] 🌡️  Température mesurée : 24.3 °C
[HUMI] 💧  Humidité mesurée    : 62 %
[TEMP] 🌡️  Température mesurée : 25.1 °C
[BOUTON] 🔘 Appui détecté ! LED ALLUMÉE 💡
[HUMI] 💧  Humidité mesurée    : 58 %
[BOUTON] 🔘 Appui détecté ! LED ÉTEINTE 🔌
```

---

## 📊 Résumé des concepts FreeRTOS manipulés

| Concept | Fonction FreeRTOS | Rôle dans ce lab |
|---|---|---|
| Mutex | `xSemaphoreCreateMutex()` | Création du Mutex pour la ressource partagée `Serial` |
| Verrouillage Mutex | `xSemaphoreTake(mutex, timeout)` | Attente et réservation exclusive |
| Déverrouillage Mutex | `xSemaphoreGive(mutex)` | Libération pour les autres tâches |
| Sémaphore Binaire | `xSemaphoreCreateBinary()` | Signalement d'événement d'une ISR vers une tâche |
| Signal depuis ISR | `xSemaphoreGiveFromISR()` | Signalement sécurisé hors contexte tâche |
