# 🗺️ Roadmap

High-level phases for this lab. For a granular, topic-by-topic status, see [PROGRESS.md](PROGRESS.md).

---

## Phase 1 — Foundations (ESP32 + Arduino)

- [x] GPIO digital I/O (`01-gpio-digital-io`) — anti-rebond non-bloquant ✅
- [x] ADC analog read (`02-adc-analog-read`) — oversampling ✅
- [x] PWM LED fade (`03-pwm-led-fade`) — LEDC hardware ✅
- [ ] Timers & interrupts
- [ ] UART / SPI / I2C 🟨 (en cours)
- [x] Wi-Fi ✅
- [x] MQTT ✅
- [x] HTTP REST APIs ✅
- [x] First reusable drivers — DHT22 ✅, HX711 ✅

## Phase 2 — Real-Time Systems 🟨 en cours

- [x] FreeRTOS tasks & queues (`01-tasks-and-queues`) ✅
- [ ] Semaphores & mutexes
- [ ] Event groups, software timers
- [ ] Multi-task ESP32 project complet

## Phase 3 — Industrial Communication

- [ ] RS-485, Modbus RTU/TCP
- [ ] CAN bus

## Phase 4 — STM32

- [ ] HAL / LL drivers
- [ ] DMA, RTC, low-power modes

## Phase 5 — Embedded Linux

- [ ] Boot process, device tree
- [ ] Kernel modules, cross compilation

## Phase 6 — Edge AI & Robotics

- [ ] TinyML / TensorFlow Lite
- [ ] ROS2 basics
- [ ] SLAM, computer vision

---

**Last Review:** July 2026
