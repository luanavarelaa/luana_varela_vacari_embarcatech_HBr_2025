# 📝 Descrição do Projeto
Nesta tarefa, foi desenvolvido um programa em linguagem C para realizar a leitura dos valores analógicos dos eixos X e Y do joystick presente na placa BitDogLab. Esses valores são convertidos digitalmente utilizando o ADC (Conversor Analógico-Digital) do RP2040.

• O programa permite exibir os valores lidos de duas formas:

    ↪ Diretamente no terminal (via UART), útil para testes e depuração.

    ↪ Ou de forma visual e centralizada no display OLED SSD1306, conectado via I2C, oferecendo uma interface amigável para o usuário.

• Essa aplicação reforça conceitos importantes de sistemas embarcados, como:

    ↪ Configuração e uso do ADC;

    ↪ Comunicação I2C;

    ↪ Manipulação de periféricos como joystick e display OLED.