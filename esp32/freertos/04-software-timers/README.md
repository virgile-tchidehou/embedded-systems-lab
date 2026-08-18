# 04 — FreeRTOS : Software Timers (Timers logiciels)

Ce projet montre comment utiliser les **Software Timers** de FreeRTOS pour exécuter des fonctions callbacks périodiques ou à retardement (One-shot) **sans créer de tâches supplémentaires** et en économisant la mémoire RAM.

---

## 🎯 Quel est l'objectif ?

- Comprendre le fonctionnement des **Timers Logiciels** dans FreeRTOS (`xTimerCreate`, `xTimerStart`, `xTimerResetFromISR`)
- Distinguer un timer **Auto-reload (Périodique)** d'un timer **One-shot (Monostable)**
- Économiser les ressources (stack RAM) en évitant la création inutile de tâches dédiées
- Respecter la règle d'or des callbacks de timers (fonctions non bloquantes)

---

## 💡 Pourquoi cette technologie est-elle importante ?

Dans un projet embarqué complexe (comme la gestion d'arrosage ou de sécurité dans **SMART-SOJA**), on a souvent besoin de temporisations :
- Faire clignoter un voyant de statut toutes les 500 ms
- Vérifier un timeout de réception Wi-Fi au bout de 5 secondes
- Éteindre un relais 10 secondes après une action

Si l'on créait une tâche FreeRTOS (`xTaskCreate`) pour chacune de ces petites fonctions, chaque tâche consommerait 2 000 à 4 000 octets de stack RAM. Avec 5 temporisations, on épuiserait rapidement la RAM du microcontrôleur.

Les **Software Timers** résolvent ce problème : ils sont tous gérés par une **seule tâche Daemon système** (`Tmr Svc`). Les callbacks s'exécutent les unes après les autres de manière très économe.

---

## 🛠️ Quel matériel est utilisé ?

| Composant | Rôle |
|---|---|
| ESP32 DevKit | Microcontrôleur |
| LED intégrée (GPIO 2) | Indicateur visuel du timer périodique (500 ms) |
| Bouton BOOT (GPIO 0) | Déclencheur d'interruption (ISR) pour le timer One-shot (3s) |

---

## ⚙️ Comment fonctionne le système ?

```
                          Tâche Daemon FreeRTOS (Tmr Svc)
                                      │
          ┌───────────────────────────┴───────────────────────────┐
          ▼                                                       ▼
   Timer Clignotant                                        Timer Sécurité
  (Auto-reload 500ms)                                    (One-shot 3000ms)
          │                                                       │
          ▼                                                       ▼
callbackTimerClignotant()                               callbackTimerSecurite()
  · Toggle LED (GPIO 2)                                   · Alerte terminée
  · Log Série ⏱️                                          · Log Série 🚨
```

1. **`timerClignotant`** (Auto-reload = `pdTRUE`) : Se réexécute automatiquement toutes les 500 ms pour inverser la LED.
2. **`timerSecurite`** (Auto-reload = `pdFALSE`) : Lancé par l'appui sur le bouton BOOT via `xTimerResetFromISR`. Il décompte 3 secondes, exécute sa callback une fois, puis se désactive.

---

## 🔁 Comment reproduire l'expérience ?

**Build & flash avec PlatformIO**

```bash
pio run -t upload
pio device monitor
```

**Comportement attendu**

```
=== 04 — FreeRTOS : Software Timers ===
-> Timer Clignotant (Périodique 500ms) démarré.
-> Appuyez sur le bouton BOOT (GPIO 0) pour déclencher le Timer One-Shot (3s).

[TIMER PERIODIQUE] ⏱️ Tick 500ms — LED ON 💡
[TIMER PERIODIQUE] ⏱️ Tick 500ms — LED OFF 🔌
[TIMER PERIODIQUE] ⏱️ Tick 500ms — LED ON 💡

(Appui sur le bouton BOOT)

[TIMER ONE-SHOT] 🚨 3 secondes écoulées — Fin de l'alerte de sécurité !
[TIMER ONE-SHOT] 🟢 Le système repasse en mode normal.
```

---

## 📊 Résumé des fonctions Software Timers

| Fonction FreeRTOS | Description |
|---|---|
| `xTimerCreate()` | Crée un timer logiciel (Périodique ou One-shot) |
| `xTimerStart(timer, blockTime)` | Démarrer un timer depuis une tâche |
| `xTimerStop(timer, blockTime)` | Arrêter un timer |
| `xTimerResetFromISR()` | Démarrer ou réinitialiser le compte à rebours d'un timer depuis une ISR |
