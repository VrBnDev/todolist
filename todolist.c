#include <ctype.h> // Funções de manipulação de caracteres
#include <stdio.h>  // Padrão C
#include <stdlib.h> // Funções para alocação de memória, conversões de strings e outras utilidades
#include <string.h> // Funções de manipulação de strings
#include "pico/binary_info.h" // 
#include "pico/cyw43_arch.h" // Módulo Wi-Fi e Bluetooth
#include "pico/stdlib.h" // Padrão Pico Pi
#include "hardware/i2c.h" // Comunicação i2c
#include "inc/ssd1306.h" // Controle de display OLED
#include "lwip/tcp.h" // Lightweight IP (lwIP) - Cria conexões 


#define LED_PIN 12          // Define o pino do LED
#define WIFI_SSID "aulaEmbarca"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "senhaEmbarca123" // Substitua pela senha da sua rede Wi-Fi

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Buffer para respostas HTTP
#define HTTP_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
                      "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='5'></head><body>" \
                      "<h1>Controle do LED</h1>" \
                      "<p><a href=\"/led/on\">Ligar LED</a></p>" \
                      "<p><a href=\"/led/off\">Desligar LED</a></p>" \
                      "</body></html>\r\n"



// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Processa a requisição HTTP
    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/on")) {
        gpio_put(LED_PIN, 1);  // Liga o LED
    } else if (strstr(request, "GET /led/off")) {
        gpio_put(LED_PIN, 0);  // Desliga o LED
    }

    // Envia a resposta HTTP
    tcp_write(tpcb, HTTP_RESPONSE, strlen(HTTP_RESPONSE), TCP_WRITE_FLAG_COPY);

    // Libera o buffer recebido
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback);  // Associa o callback HTTP
    return ERR_OK;
}

// Função de setup do servidor TCP
static void start_http_server(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Liga o servidor na porta 80
    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);  // Coloca o PCB em modo de escuta
    tcp_accept(pcb, connection_callback);  // Associa o callback de conexão

    printf("Servidor HTTP rodando na porta 80...\n");
}

int main() {
    stdio_init_all();  // Inicializa a saída padrão

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Erro ao inicializar o Wi-Fi\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        return 1;
    }else {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("Endereço IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    printf("Wi-Fi conectado!\n");
    printf("Para ligar ou desligar o LED acesse o Endereço IP seguido de /led/on ou /led/off\n");

    
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");

    // Configura o LED como saída
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Inicia o servidor HTTP
    start_http_server();
    
    // Inicialização do I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do OLED
    ssd1306_init();

    // Preparando a área de renderização do display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Mensagem de teste

    char *teste[] = {
        "Ola, mundo!"
    };

    int y = 0;
    for (uint i = 0; i < count_of(teste); i++)
    {
        ssd1306_draw_string(ssd, 5, y, teste[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);

    // Loop principal
    while (true) {
        cyw43_arch_poll();  // Necessário para manter o Wi-Fi ativo
        sleep_ms(100);
    }

    

    cyw43_arch_deinit();  // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;
}