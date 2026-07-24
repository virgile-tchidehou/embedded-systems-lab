# 03 — FreeRTOS : Event Groups (Groupes d'événements)

Trois tâches d'initialisation indépendantes s'exécutent en parallèle et signalent leur état à l'aide d'un **Event Group**. Une tâche de supervision attend que **toutes** les conditions soient remplies pour autoriser le démarrage global du système.

---

## 🎯 Quel est l'objectif ?

- Découvrir les **Event Groups** dans FreeRTOS (`xEventGroupCreate`)
- Utiliser les drapeaux binationaux (bits d'événements) pour représenter des états (`xEventGroupSetBits`)
- Réaliser une **barrière de synchronisation multi-tâches** avec `xEventGroupWaitBits`
- Comprendre la combinaison logique des bits (opérateurs ET / OU) pour débloquer une tâche

---

## 💡 Pourquoi cette technologie est-elle importante ?

Dans les systèmes embarqués réels (comme une station météo intelligente ou un automate agricole type **SMART-SOJA**), le système ne peut pas démarrer la production tant que :
1. Le **Wi-Fi** n'est pas connecté
2. Les **capteurs** (température, humidité, sol) ne sont pas autotestés et prêts
3. Les **actionneurs** (pompes, vannes, moteurs) n'ont pas validé leur position initiale

Plutôt que de vérifier chaque composant de manière séquentielle (ce qui serait lent et bloquant), FreeRTOS permet d'exécuter ces vérifications **en parallèle**. 

L'**Event Group** sert de panneau d'affichage central où chaque tâche vient lever son drapeau (son Bit). La tâche principale attend que tous les drapeaux soient levés pour démarrer l'application.

---

## 🛠️ Quel matériel est utilisé ?

| Composant | Rôle |
|---|---|
| ESP32 DevKit | Microcontrôleur (double cœur Xtensa LX6) |
| LED intégrée (GPIO 2) | Témoin d'activation globale du système une fois tous les bits validés |

---

## ⚙️ Comment fonctionne le système ?

```
Tâche Wi-Fi        (2.0s) ───► Activer BIT_WIFI     (Bit 0 : 0b001) ──┐
Tâche Capteurs     (3.5s) ───► Activer BIT_CAPTEURS  (Bit 1 : 0b010) ──┼──► Event Group
Tâche Moteur       (1.0s) ───► Activer BIT_MOTEUR    (Bit 2 : 0b100) ──┘
                                                                            │
                                                                            ▼
                                                                tache_superviseur
                                                         WaitBits(0b111, waitForAll=TRUE)
                                                                            │
                                                                            ▼
                                                                  🚀 SYSTÈME OPÉRATIONNEL
                                                                      (LED allumée)
```

1. **`tache_moteur`** se termine au bout de 1s et active le `BIT_MOTEUR` (`0b100`).
2. **`tache_wifi`** se termine au bout de 2s et active le `BIT_WIFI` (`0b001`).
3. **`tache_capteurs`** se termine au bout de 3.5s et active le `BIT_CAPTEURS` (`0b010`).
4. **`tache_superviseur`**, bloquée sur `xEventGroupWaitBits`, s'éveille à 3.5s exactes dès que le dernier bit (`BIT_CAPTEURS`) est levé. Elle allume la LED et lance le fonctionnement normal.

---

## 🔁 Comment reproduire l'expérience ?

**Build & flash avec PlatformIO**

```bash
pio run -t upload
pio device monitor
```

**Comportement attendu**

```
=== 03 — FreeRTOS : Event Groups ===
Démarrage de la séquence d'initialisation multi-tâches...

[SUPERVISEUR] ⏳ En attente de l'initialisation complète...
[WIFI] 📡 Connexion Wi-Fi en cours...
[CAPTEURS] 🔍 Autotest des capteurs...
[MOTEUR] ⚙️ Test du driver moteur...
[MOTEUR] ✅ Moteur opérationnel !
[WIFI] ✅ Wi-Fi connecté avec succès !
[CAPTEURS] ✅ Capteurs prêts et calibrés !

==============================================
🚀 SYSTÈME PRÊT ! Tous les sous-systèmes sont OK.
==============================================

[SUPERVISEUR] 🟢 Système en cours de fonctionnement...
```

---

## 📊 Résumé des fonctions Event Group utilisées

| Fonction FreeRTOS | Description |
|---|---|
| `xEventGroupCreate()` | Alloue et initialise un groupe d'événements |
| `xEventGroupSetBits(group, bits)` | Active un ou plusieurs bits dans le groupe |
| `xEventGroupWaitBits(group, bits, clear, waitForAll, timeout)` | Suspend la tâche jusqu'à ce que les bits spécifiés soient activés |
