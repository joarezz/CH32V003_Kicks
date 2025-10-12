//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK : CH32VFUN
//12/10/2025
//https://www.youtube.com/@kickstech
//
// Exemplo com código gerando 3 numeros randomicos com duas casas decimais para teste em app bluetooth
// APP: https://play.google.com/store/apps/details?id=io.kodular.joarez_fonseca.Monitor_Serial

#include "ch32v003fun.h"  // Biblioteca base do CH32V003
#include "lib_uart.h"     // Biblioteca da UART
#include "timers_utils.h" // Biblioteca de utilitários de tempo (millis, micros)
#include <stdio.h>        // Para a função snprintf()

// --- Nossa "Máquina de Sorteio" de Números ---
// Para os alunos: "Não se preocupem em como isso funciona agora,
// apenas saibam que a função my_rand() nos dará um número aleatório!"
static unsigned long g_seed = 1;
#define RAND_MAX 32767
void my_srand(unsigned int seed) { g_seed = seed; }
int my_rand(void)
{
    g_seed = g_seed * 1103515245 + 12345;
    return ((unsigned int)(g_seed / 65536) % (RAND_MAX + 1));
}
// --- Fim da "Máquina de Sorteio" ---

/**
 * @brief Gera uma string completa com 3 valores aleatórios formatados.
 * Exemplo de retorno: "GG;12,34;56,78;90,12;GG"
 * @param buffer Onde a string final será armazenada.
 * @param buffer_size O tamanho máximo do buffer para segurança.
 */
void gerarPacoteDeDados(char *buffer, size_t buffer_size)
{
    // Sorteia três números inteiros, cada um entre 0 e 9999.
    // Pense neles como leituras de 3 sensores imaginários.
    int valor_sensor_1 = my_rand() % 10000; // Ex: 2573
    int valor_sensor_2 = my_rand() % 10000; // Ex: 809
    int valor_sensor_3 = my_rand() % 10000; // Ex: 9810

    // A mágica do snprintf(): Monta a string final de uma só vez!
    // O truque "%d,%02d" divide um número como 2573 em "25,73".
    // (parte inteira = 2573 / 100) e (parte decimal = 2573 % 100)
    snprintf(buffer, buffer_size, "GG;%d,%02d;%d,%02d;%d,%02d;GG",
             valor_sensor_1 / 100, valor_sensor_1 % 100,
             valor_sensor_2 / 100, valor_sensor_2 % 100,
             valor_sensor_3 / 100, valor_sensor_3 % 100);
}

/**
 * @brief setup() é executada uma vez para configurar o microcontrolador.
 */
void setup()
{
    SystemInit();
    setupTimer1(); // Necessário para as funções de tempo

    // "Embaralha" nosso gerador de números aleatórios usando o tempo atual.
    // Isso garante que os números sejam diferentes a cada vez que o chip é ligado.
    my_srand(micros());

    // Inicializa a comunicação serial com o computador.
    Serial_begin(115200);
    Delay_Ms(100);


}

/**
 * @brief main() é o coração do programa, executado em loop infinito.
 */
int main(void)
{
    setup();

    unsigned long ultima_vez_que_enviei = 0;
    const long intervalo_de_envio = 1000; // 1 segundo

    while (1)
    {
        // --- TAREFA 1: ENVIAR DADOS A CADA SEGUNDO ---
        if (millis() - ultima_vez_que_enviei >= intervalo_de_envio)
        {
            ultima_vez_que_enviei = millis(); // Atualiza o tempo do último envio

            char pacote_de_dados[80]; // Cria um espaço na memória para a mensagem

            // Chama nossa função para criar o pacote de dados com números aleatórios
            gerarPacoteDeDados(pacote_de_dados, sizeof(pacote_de_dados));

            // Envia o pacote de dados prontinho pela serial!
            Serial_println(pacote_de_dados);
        }

        // --- TAREFA 2: OUVIR E RESPONDER (ECO) ---
        char *linha_recebida = Serial_readString();
        if (linha_recebida != NULL)
        {
            // Se recebemos algo, vamos responder.
            char buffer_resposta[80];
            snprintf(buffer_resposta, sizeof(buffer_resposta), "Eco: %s", linha_recebida);
            Serial_println(buffer_resposta);
        }
    }
}
