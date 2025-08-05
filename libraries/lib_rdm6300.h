
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_RDM6300_H
#define LIB_RDM6300_H

#include <stdint.h> // Para uint8_t, uint32_t
#include <stdbool.h> // Para bool

// Definicoes de tempo para o RDM6300
// Note: Estes definem estao aqui para que possam ser vistos por outras partes do codigo,
// mas a logica de uso esta encapsulada na lib_rdm6300.c
#define PACKET_TIMEOUT_MS        500  // 500ms para receber um pacote completo
#define TAG_REMOVED_DEBOUNCE_MS 500 // A tag sera considerada removida apos 1 segundo sem nenhum byte

// Funcao para inicializar a logica de processamento do RDM6300.
// Deve ser chamada uma vez no inicio do programa (no main()).
void RDM6300_Init(void);

// Funcao para processar um unico byte recebido da UART.
// Deve ser chamada sempre que um byte for lido do modulo RDM6300.
void RDM6300_ProcessByte(uint8_t received_byte);

// Funcao para atualizar o estado interno da biblioteca (timeouts, debounce).
// Deve ser chamada periodicamente no loop principal (no main()),
// passando o tempo atual em milissegundos (obtido de millis()).
void RDM6300_UpdateState(uint32_t current_millis);

// Retorna true se um novo ID de tag valido acabou de ser detectado e esta disponivel.
// Esta flag e limpa apos esta funcao ser chamada.
bool RDM6300_IsNewTagAvailable(void);

// Retorna um ponteiro para a string que contem o ultimo ID de tag valido.
// A string e gerenciada pela biblioteca e nao deve ser modificada pelo chamador.
// Retorna uma string vazia se nenhum ID estiver disponivel ou se a tag foi removida.
const char* RDM6300_GetTagID(void);

// Retorna true se o sistema atualmente considera que uma tag esta no campo do leitor.
bool RDM6300_IsTagCurrentlyInField(void);

// Retorna true se um evento de "Tag Removida" acabou de ocorrer.
// Esta flag e limpa apos esta funcao ser chamada.
bool RDM6300_IsTagRemovedEvent(void);

#endif // LIB_RDM6300_H
