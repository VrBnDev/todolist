# ✅ Integrando o TodoIst a BitDogLab

## Bibliotecas Utilizadas 

- ### Padrão do C
    - **<ctype.h>** - Para manipulação de caracteres (ex.: verificar se é letra ou número, converter maiúsculas/minúsculas);
    - **<stdio.h>** - Entradas e saídas padrão (ex.: printf, scan);
    - **<stdlib.h>** - Funções como alocação de memória, conversões (atoi, malloc);
    - **<string.h>** - Para manipulação de strings (strlen, strcpy).

- ### Raspberry Pi Pico
    - **<pico/binary_info.h>** - Informações sobre o firmware, útil para depuração;
    - **<pico/cyw43_arch.h>** - Gerencia o Wi-Fi e Bluetooth no RP2040
    - **<pico/stdlib.h>** - Funções básicas do Pi Pico

-  ### Comunicação e Hardware
    - **<hardware/i2c.h>** - Gerencia a comunição i2c (nesse código usado para o OLED)
    - **<inc/ssd1306.h>** - Controla displays OLED baseados no driver ssd1306
    - **<lwip/tpc.h>** - Cria conexões TCP/IP no Pico W. 


## Componentes utilizados

- 📟 Display OLED 
- 📟 Módulo WiFi
- 📟 Botões da BitDogLab

## Objetivos

- ✅ Conectar o módulo WiFi a rede
- ✅ Ligar OLED
- ⬜ Integrar a BitDogLab ao software ToDoIst
- ⬜ Exibir lista de tarefas no OLED

## Referências

- Exemplo de manipulação do OLED: https://github.com/BitDogLab/BitDogLab-C/tree/main/display_oled
- Artigo: https://www.makerhero.com/blog/desenvolvendo-um-sistema-de-lista-de-tarefas-com-raspberry-pi-pico-e-display-e-paper/ 