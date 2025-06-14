# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Luana Varela Vacari**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, 13 de junho de 2025

---

Este projeto implementa um sistema multitarefa embarcado utilizando o sistema operacional de tempo real (RTOS) FreeRTOS na placa BitDogLab. O objetivo é demonstrar o controle concorrente de múltiplos periféricos através da criação e gerenciamento de tarefas independentes.


## 🚀 Funcionalidades

O sistema é composto por três tarefas principais que rodam de forma concorrente:

1.  **Controle do LED RGB (`led_task`)**: Alterna ciclicamente entre as cores vermelho, verde e azul.
2.  **Controle do Buzzer (`buzzer_task`)**: Emite um bipe sonoro de forma periódica.
3.  **Monitoramento dos Botões (`button_task`)**:

        ↪ Botão A: Controla a tarefa do LED. Suspende a tarefa enquanto o botão está pressionado e a retoma ao ser solto.

        ↪ Botão B: Controla a tarefa do buzzer. Suspende a tarefa enquanto o botão está pressionado e a retoma ao ser solto.

## 🛠️ Hardware e Software

* **Hardware**: Placa BitDogLab com Raspberry Pi Pico W.
* **Software**:
    * Linguagem: C
    * RTOS: FreeRTOS
    * SDK: Raspberry Pi Pico SDK
    * Build System: CMake



## 📐 Arquitetura e Funcionamento do Código

O projeto foi desenvolvido de forma modular para promover a organização e a manutenibilidade.

* `src/main.c`: É o ponto de entrada. Sua única responsabilidade é criar as três tarefas (`xTaskCreate`) e entregar o controle do processador ao escalonador do FreeRTOS (`vTaskStartScheduler`).

* `src/controle_led.c`: Contém a lógica da `led_task`. Em um loop infinito, ela acende uma cor do LED e chama `vTaskDelay()`, uma função do RTOS que pausa a tarefa e libera a CPU para outras tarefas.

* `src/controle_buzzer.c`: Contém a lógica da `buzzer_task`. Utiliza o hardware PWM do Pico para gerar um tom. A função `beep` controla a duração do som, e a tarefa utiliza `vTaskDelay()` para criar a pausa entre os bipes, cedendo o controle da CPU.

* `src/controle_botao.c`: Contém a lógica da `button_task`. Esta tarefa verifica continuamente o estado dos botões. Ao detectar um botão pressionado ou solto, ela utiliza `vTaskSuspend()` para pausar a tarefa alvo (LED ou buzzer) e `vTaskResume()` para retomá-la. É a peça central da interatividade do sistema.

### O Papel do FreeRTOS

O FreeRTOS é o "cérebro" da operação, gerenciando qual tarefa deve ser executada a cada momento.

1.  **Escalonamento por Prioridades**: A `button_task` foi configurada com uma prioridade maior (`2`) do que as tarefas do LED e do buzzer (prioridade `1`). Isso garante que, se você apertar um botão, o FreeRTOS irá **imediatamente pausar** a tarefa que estiver rodando (seja a do LED ou a do buzzer) para executar a `button_task`. Isso torna a interface com o usuário extremamente responsiva.

2.  **Cooperação com `vTaskDelay()`**: As tarefas do LED e do buzzer passam a maior parte do tempo "dormindo", em um estado de bloqueio, graças à função `vTaskDelay()`. Elas não usam loops de espera que consomem CPU (`busy-waiting`). Em vez disso, elas pedem ao escalonador para serem acordadas após um certo tempo, permitindo que a CPU seja usada por outras tarefas ou entre em modo de baixo consumo.

3.  **Comunicação entre Tarefas**: A `button_task` controla as outras duas através dos seus "handles" (ponteiros de controle) e das funções `vTaskSuspend()` e `vTaskResume()`. Isso demonstra um mecanismo simples e eficaz de comunicação e controle entre tarefas em um RTOS.

### 📹 Vídeo de Demonstração

https://youtu.be/v3bCy_3M94I
---

## 📜 Licença
GNU GPL-3.0.