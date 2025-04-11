# 📝 Descrição do Projeto
Este projeto implementa um contador decrescente com controle por interrupções, utilizando dois botões (GPIO5 e GPIO6) e um display OLED. O comportamento do sistema é o seguinte:

• Ao pressionar o Botão A (GPIO5):

    ↪ O contador é reiniciado para 9 e exibido no display OLED.

    ↪ Inicia-se uma contagem regressiva ativa, decrementando de 1 em 1 a cada segundo.

    ↪ Durante a contagem, são registradas as pressões no Botão B (GPIO6).

• Quando o contador chega a 0:

    ↪ O sistema congela, ignorando cliques futuros no Botão B.

    ↪ O display mostra o valor 0 e o total de cliques no Botão B durante a contagem.

    ↪ O sistema só reinicia quando o Botão A for pressionado novamente, resetando o contador para 9 e os cliques para 0.