# 📝 Descrição do Projeto
Este projeto demonstra o uso da arquitetura modular em C para a Raspberry Pi Pico W, implementando o controle de um LED embutido através de três camadas principais:

    ↪ Driver (acesso ao hardware),

    ↪ HAL (abstração do hardware),

    ↪ App (lógica principal).
Funcionamento: 

    • A aplicação inicializa a Pico W e faz o LED embutido piscar a cada 500ms.

    • O controle do LED é feito de forma modular, separando claramente cada responsabilidade.
    
## 📂 Estrutura do Projeto
    projeto/
    ├── app/
    │   └── main.c
    ├── drivers/
    │   └── led_embutido.c
    ├── hal/
    │   └── hal_led.c
    ├── include/
    │   ├── led_embutido.h
    │   └── hal_led.h
    ├── CMakeLists.txt

• app/main.c: Contém o fluxo principal da aplicação.

• hal/hal_led.c: Implementa funções de alto nível para o controle do LED.

• drivers/led_embutido.c: Faz o controle direto do LED via API cyw43_arch.

• include/: Guarda os arquivos de cabeçalho (.h) que descrevem as interfaces das funções.

    