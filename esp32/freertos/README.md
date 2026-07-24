# ESP32 — FreeRTOS

Tasks, queues, semaphores, mutexes, event groups, timers.

| # | Expérimentation | Concepts clés |
|---|---|---|
| [01](01-tasks-and-queues/) | Tasks & Queue | `xTaskCreatePinnedToCore`, `xQueueCreate`, `vTaskDelay`, priorités |
| [02](02-semaphores-and-mutexes/) | Semaphores & Mutexes | `xSemaphoreCreateBinary`, `xSemaphoreCreateMutex`, accès concurrent |
| [03](03-event-groups/) | Event Groups | `xEventGroupCreate`, synchronisation multi-tâches |
| [04](04-software-timers/) | Software Timers | `xTimerCreate`, callbacks périodiques sans tâche dédiée |
| 05 | Projet multi-tâches | Intégration de tous les patterns sur un cas réel |
