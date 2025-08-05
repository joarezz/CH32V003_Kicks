
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lib_rdm6300.h" // Inclui o proprio cabecalho
#include <string.h>   // Para memset, strncpy, strcmp
#include <stdio.h>    // Nao mais necessario se os prints de debug forem removidos, mas mantido por precaucao
                      // ou se for adicionar um print de debug interno novamente.

// --- Variaveis Globais (internas a esta biblioteca) ---
#define EXPECTED_PACKET_SIZE 14 // Tamanho fixo do pacote do RDM6300

static uint8_t rdm_packet_buffer[EXPECTED_PACKET_SIZE];
static char tag_data_ascii[10 + 1]; 
static char tag_id_extracted[8 + 1]; 
static char last_processed_tag_id[9]; // Armazena o ultimo ID processado pela lib

static bool _tag_currently_in_field = false; 
static bool _new_tag_available_flag = false; // Flag para sinalizar que um novo ID esta disponivel
static bool _tag_removed_event_flag = false; // Flag para sinalizar que uma tag foi removida

// Variaveis de estado para o processamento de pacotes
static uint32_t _last_byte_time = 0;
static uint32_t _tag_not_detected_timer = 0;

typedef enum {
    WAITING_FOR_STX,
    READING_PACKET_DATA
} RDM_RX_STATE;

static RDM_RX_STATE _rx_state = WAITING_FOR_STX;
static uint8_t _packet_idx = 0;

// --- Funcoes Auxiliares (internas a esta biblioteca) ---
static uint8_t hex_char_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static uint8_t hex_to_byte(const char *hex_str) {
    return (hex_char_to_byte(hex_str[0]) << 4) | hex_char_to_byte(hex_str[1]);
}

static uint8_t calculate_checksum_from_data(const char* data_ascii_10_chars) {
    uint8_t checksum = 0;
    for (int i = 0; i < 10; i += 2) {
        checksum ^= hex_to_byte(data_ascii_10_chars + i);
    }
    return checksum;
}

// --- Implementacao das Funcoes Publicas ---

void RDM6300_Init(void) {
    memset(last_processed_tag_id, 0, sizeof(last_processed_tag_id));
    _tag_currently_in_field = false;
    _new_tag_available_flag = false;
    _tag_removed_event_flag = false;
    _last_byte_time = 0;
    _tag_not_detected_timer = 0;
    _rx_state = WAITING_FOR_STX;
    _packet_idx = 0;
}

void RDM6300_ProcessByte(uint8_t received_byte) {
    // Atualiza o tempo do ultimo byte recebido (para timeout de pacote)
    // Note: current_millis e obtido em RDM6300_UpdateState, nao aqui.
    // A logica aqui e apenas sobre o processamento do byte.
    // _last_byte_time sera atualizado em RDM6300_UpdateState.
    
    // Resetar o timer de "tag nao detectada" pois um byte foi recebido
    _tag_not_detected_timer = 0; 
    _tag_currently_in_field = true; // Uma tag esta sendo lida (ou pelo menos ha bytes vindo)


    switch (_rx_state)
    {
        case WAITING_FOR_STX:
            if (received_byte == 0x02) // STX (Start of Text)
            {
                rdm_packet_buffer[0] = received_byte;
                _packet_idx = 1;
                _rx_state = READING_PACKET_DATA;
            }
            break;

        case READING_PACKET_DATA:
            if (_packet_idx >= EXPECTED_PACKET_SIZE) {
                // Erro: Buffer overflow antes do ETX
                _rx_state = WAITING_FOR_STX;
                _packet_idx = 0;
                break;
            }
            
            rdm_packet_buffer[_packet_idx++] = received_byte;

            if (_packet_idx == EXPECTED_PACKET_SIZE) 
            {
                if (rdm_packet_buffer[EXPECTED_PACKET_SIZE - 1] != 0x03) { // ETX (End of Text)
                    // Erro: Pacote malformado - ETX invalido
                    _rx_state = WAITING_FOR_STX; 
                    _packet_idx = 0;
                    break; 
                }

                // Extrair os 10 caracteres ASCII da tag
                for (int i = 0; i < 10; i++)
                {
                    tag_data_ascii[i] = rdm_packet_buffer[i + 1];
                }
                tag_data_ascii[10] = '\0'; 

                // Extrair os 8 caracteres do ID da tag (posicoes 2 a 9 da string tag_data_ascii)
                strncpy(tag_id_extracted, tag_data_ascii + 2, 8); 
                tag_id_extracted[8] = '\0'; 

                // Extrair o checksum reportado
                char reported_checksum_ascii[3];
                reported_checksum_ascii[0] = rdm_packet_buffer[11];
                reported_checksum_ascii[1] = rdm_packet_buffer[12];
                reported_checksum_ascii[2] = '\0'; 
                uint8_t reported_checksum_byte = hex_to_byte(reported_checksum_ascii);

                // Calcular o checksum
                uint8_t calculated_checksum_byte = calculate_checksum_from_data(tag_data_ascii);
                
                if (calculated_checksum_byte != reported_checksum_byte) {
                    // Erro: Checksum mismatch
                    _rx_state = WAITING_FOR_STX;
                    _packet_idx = 0;
                    break; 
                }
                
                // Pacote valido e checksum correto
                if (strcmp(tag_id_extracted, last_processed_tag_id) != 0) {
                    strncpy(last_processed_tag_id, tag_id_extracted, 8);
                    last_processed_tag_id[8] = '\0';
                    _new_tag_available_flag = true; // Sinaliza que um novo ID esta disponivel
                    _tag_removed_event_flag = false; // Uma tag foi lida, entao nao ha evento de "removida" pendente
                }
                
                _rx_state = WAITING_FOR_STX; // Resetar para o proximo pacote
                _packet_idx = 0;
            }
            break;
    }
}

void RDM6300_UpdateState(uint32_t current_millis) {
    _last_byte_time = current_millis; // Atualiza o tempo do ultimo "processamento" de byte (ou check)

    // Logica para timeout de recepcao de pacote (se estiver no meio da leitura)
    if (_rx_state == READING_PACKET_DATA && (current_millis - _last_byte_time > PACKET_TIMEOUT_MS)) {
        _rx_state = WAITING_FOR_STX;
        _packet_idx = 0;
    }

    // Logica para debounce de "Tag Removed"
    if (_tag_currently_in_field) {
        // Inicia o timer na primeira vez que nao ha bytes chegando
        if (_tag_not_detected_timer == 0) { 
            _tag_not_detected_timer = current_millis;
        }
        
        // Se o tempo sem detectar bytes exceder o limite de debounce
        if ((current_millis - _tag_not_detected_timer) > TAG_REMOVED_DEBOUNCE_MS) {
            _tag_currently_in_field = false;
            memset(last_processed_tag_id, 0, sizeof(last_processed_tag_id)); // Limpa o ID da tag
            _tag_removed_event_flag = true; // Sinaliza o evento de tag removida
            _tag_not_detected_timer = 0; // Reseta o timer apos a deteccao
        }
    }
}

bool RDM6300_IsNewTagAvailable(void) {
    if (_new_tag_available_flag) {
        _new_tag_available_flag = false; // Limpa a flag apos ser lida
        return true;
    }
    return false;
}

const char* RDM6300_GetTagID(void) {
    return (const char*)last_processed_tag_id;
}

bool RDM6300_IsTagCurrentlyInField(void) {
    return _tag_currently_in_field;
}

bool RDM6300_IsTagRemovedEvent(void) {
    if (_tag_removed_event_flag) {
        _tag_removed_event_flag = false; // Limpa a flag apos ser lida
        return true;
    }
    return false;
}