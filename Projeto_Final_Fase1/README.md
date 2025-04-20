# 🐋 Projeto Whale — Monitoramento de Ruído Para Ambientes Inclusivos

Este projeto foi desenvolvido como parte do módulo final do curso EmbarcaTech. A proposta é criar um sistema embarcado com a BitDogLab para auxiliar alunos autistas com sensibilidade auditiva em ambientes escolares.

    O sistema monitora em tempo real o nível de ruído no ambiente utilizando o microfone embutido na placa. Quando o som ultrapassa um limite pré-estabelecido, o dispositivo aciona alertas visuais por meio de um LED e mensagens na tela OLED, promovendo um ambiente mais acolhedor e seguro para estudantes neurodivergentes.

O LED sinaliza o nível de ruído:

    🟢 Verde: ruído dentro do limite aceitável.

    🔴 Vermelho: ruído acima do limite.

O sistema também permite o ajuste dinâmico da sensibilidade com os botões físicos da BitDogLab:

    • Botão A: diminui o limite de ruído em 0.1.

    • Botão B: aumenta o limite de ruído em 0.1.

    O valor inicial do limite é 1.0, podendo variar entre 0.2 e 1.8.

Todo o projeto foi desenvolvido utilizando apenas os recursos integrados da BitDogLab, aplicando conhecimentos aprendidos ao longo do curso, como:

    ↪ Manipulação de GPIOs

    ↪ Leitura de sinais analógicos (ADC)

    ↪ Comunicação via I2C

    ↪ Temporizadores



