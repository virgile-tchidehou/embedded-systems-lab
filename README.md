<div align="center">

# ⚙️ Embedded Systems Lab

### Laboratoire personnel dédié aux systèmes embarqués, au temps réel et à l’intégration matériel/logiciel

[![Licence: MIT](https://img.shields.io/badge/Licence-MIT-2ea44f.svg)](LICENSE)
![Statut](https://img.shields.io/badge/Statut-Apprentissage%20actif-blue.svg)
![Langage](https://img.shields.io/badge/C-C%2FC%2B%2B-00599C.svg?logo=cplusplus)
![Plateforme](https://img.shields.io/badge/Plateforme-ESP32-E7352C.svg?logo=espressif&logoColor=white)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-e6522c.svg)
![Arduino](https://img.shields.io/badge/Framework-Arduino-00979D.svg)

**Systèmes embarqués • RTOS • Drivers • Communications • IoT • Intégration matériel/logiciel**

</div>

---

# 📖 Présentation

Ce dépôt constitue mon laboratoire personnel d’apprentissage et d’expérimentation autour des systèmes embarqués, à travers la programmation, le prototypage matériel, les essais et la documentation technique.

Les travaux actuels sont principalement centrés sur **ESP32, C/C++, FreeRTOS, les périphériques et les protocoles de communication**. Les plateformes et sujets plus avancés sont intégrés progressivement au fil de ma montée en compétences.

L’objectif n’est pas d’accumuler des exemples isolés, mais de construire un ensemble d’expériences suivant une démarche d’ingénierie cohérente : comprendre, concevoir, implémenter, tester et documenter.

---

# 🎯 Objectifs

Ce laboratoire me permet de :

- renforcer mes compétences en programmation embarquée C/C++ ;
- approfondir le fonctionnement des microcontrôleurs et l’interaction matériel/logiciel ;
- développer des applications temps réel avec FreeRTOS ;
- explorer les protocoles de communication utilisés dans les systèmes embarqués et industriels ;
- concevoir progressivement des drivers réutilisables et de petits systèmes embarqués ;
- documenter les expériences, résultats et limites rencontrées ;
- préparer des travaux plus avancés en robotique, IoT et Linux embarqué.

---

# 🧠 Méthode de travail

Chaque expérience suit une démarche simple :

1. Comprendre le concept
2. Définir le comportement attendu
3. Implémenter le firmware
4. Tester sur le matériel
5. Valider le résultat
6. Documenter les observations et pistes d’amélioration

L’objectif n’est pas seulement de faire fonctionner un système, mais de comprendre **comment et pourquoi il fonctionne**.

---

# 📁 Structure du dépôt

```text
embedded-systems-lab/
│
├── README.md
├── PROGRESS.md
├── ROADMAP.md
├── LICENSE
│
├── docs/
├── esp32/
│   ├── fundamentals/
│   ├── peripherals/
│   ├── communication/
│   ├── networking/
│   ├── freertos/
│   └── projects/
├── stm32/
├── raspberry-pi-pico/
├── embedded-linux/
├── communication/
├── drivers/
├── shared/
├── media/
└── tools/
```

Certains dossiers correspondent à des axes d’apprentissage prévus et peuvent rester incomplets tant que le sujet concerné n’a pas encore été abordé activement.

---

# 🔬 Axes actuellement travaillés

## ESP32

Sujets déjà abordés ou en cours :

- GPIO
- ADC
- PWM
- interruptions et timers
- UART
- I²C
- SPI
- Wi-Fi
- MQTT
- FreeRTOS

## FreeRTOS

Le laboratoire couvre progressivement :

- tâches ;
- files de messages ;
- sémaphores ;
- mutex ;
- groupes d’événements ;
- timers logiciels ;
- synchronisation et communication entre tâches.

## Communications

Protocoles actuels et prévus :

- UART
- I²C
- SPI
- MQTT
- CAN Bus
- RS485
- Modbus

---

# 📈 Progression

### Acquis / pratiqués actuellement

- [x] Fondamentaux ESP32
- [x] GPIO
- [x] ADC
- [x] PWM
- [x] Wi-Fi
- [x] MQTT
- [x] Fondamentaux FreeRTOS

### Prochaines étapes

- [ ] FreeRTOS avancé
- [ ] ESP-IDF
- [ ] STM32
- [ ] CAN Bus
- [ ] Linux embarqué
- [ ] Zephyr RTOS

---

# 🚀 Direction à long terme

Ce laboratoire évoluera progressivement vers :

- Linux embarqué ;
- communications industrielles ;
- robotique ;
- Edge AI / TinyML ;
- sûreté de fonctionnement ;
- systèmes cyber-physiques.

---

<div align="center">

# 👨‍💻 Auteur

**Dodji Virgile TCHIDEHOU**

Diplômé en Informatique Industrielle et Maintenance

Systèmes embarqués • Mécatronique • IoT • Robotique

</div>

---

# 📄 Licence

Ce projet est distribué sous licence MIT.
