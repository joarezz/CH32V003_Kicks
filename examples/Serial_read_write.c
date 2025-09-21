#include "ch32v003fun.h" // Biblioteca base do CH32V003
#include "lib_uart.h"     // Biblioteca da UART
#include "timers_utils.h" // Biblioteca de utilitários de tempo (para Delay_Ms)
#include <stdio.h>        // Para a função sprintf()
#include <string.h>       // Para a função strlen()

// Declaração de um buffer para o recebimento de dados da UART.
// O tamanho deve ser grande o suficiente para a maior string que você espera receber.
// Lembre-se que o buffer é circular e a biblioteca irá gerenciar a leitura e escrita.
#define RX_BUFFER_SIZE 64
static uint8_t rx_buffer[RX_BUFFER_SIZE]; // Este buffer é usado internamente pela lib_uart

void setup()
{
    // Inicializa o sistema do microcontrolador.
    SystemInit();

    // Configura o Timer 1 para as funções de tempo, como Delay_Ms.
    setupTimer1();

    // 1. Inicializa a comunicação serial (UART) a 115200 bps (bits por segundo).
    // Esta função usa o pino RX (PD6) e TX (PD5) por padrão.
    // O "Serial_begin" é um alias conveniente para "uart_init" com configurações padrão.
    Serial_begin(115200);

    // Espera um momento para o monitor serial ser aberto e se conectar.
    Delay_Ms(500);

    // Envia uma mensagem de boas-vindas para o monitor serial.
    // A função "Serial_println" adiciona automaticamente uma quebra de linha (\r\n).
    Serial_println("--- Exemplo de Comunicacao Serial Corrigido ---");
    Serial_println("Envie linhas de texto terminadas por Enter.");
    Serial_println("-------------------------------------");

    // Limpa o buffer de recebimento da UART no início, caso haja dados "sujos".
    // Note que a lib_uart já gerencia seu buffer interno. Esta chamada é mais
    // para garantir que o programa comece "limpo" se houver algum resquício.
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
}

int main(void)
{
    setup();

    while (1)
    {
        // 1. IMPRIMIR INFORMACOES (tipo uma string para um monitor serial)
        // A cada 2 segundos, exibe uma mensagem no monitor serial.
        static unsigned long last_print_time = 0;
        if (millis() - last_print_time >= 2000)
        {
            last_print_time = millis();

            // Exemplo de como formatar e enviar valores de variáveis.
            char print_buf[64];
            int valor = 123;
            // sprintf() formata a string e armazena no buffer 'print_buf'.
            sprintf(print_buf, "Tempo: %lu ms. Valor: %d", millis(), valor);
            Serial_println(print_buf);
        }

        // 3. LEITURA DE UMA STRING (utilizando uma terminação)
        // A função Serial_readString() procura por uma linha completa no buffer.
        // Ela retorna um ponteiro para a string lida, ou NULL se nenhuma linha estiver completa.
        // Esta é a forma recomendada de ler dados de texto enviados linha a linha.
        char* received_line = Serial_readString();

        if (received_line != NULL)
        {
            // Se uma string completa foi recebida...
            Serial_println("--- Linha recebida: ---");
            Serial_println(received_line); // Exibe a string lida.
            Serial_println("--- Fim da linha ---");

            // A função Serial_readString gerencia a limpeza do buffer de linha
            // internamente para a próxima chamada. Não é necessário limpar aqui.
        }


    }
}
