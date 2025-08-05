
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#ifndef LIB_I2C_H
#define LIB_I2C_H

#include "ch32v003fun.h" // Para GPIO_Mode, GPIO_Pin, etc.
#include <stdint.h>      // Para uint8_t, uint32_t
#include <stdbool.h>     // Para bool


// --- Definições Internas para Funções I2C (ACK/NACK, TX/RX) ---
#define I2C_ACK             0x00
#define I2C_NACK            0x01
#define I2C_TX              0x00
#define I2C_RX              0x01

#ifdef __cplusplus
extern "C" {
#endif

// --- Definições de Erro I2C ---
typedef enum {
    I2C_OK = 0,
    I2C_ERR_INIT,
    I2C_ERR_TIMEOUT,
    I2C_ERR_START,
    I2C_ERR_ADDR_TX,
    I2C_ERR_TX_BYTE,
    I2C_ERR_RX_BYTE,
    I2C_ERR_STOP,
    I2C_ERR_NACK_RX_LAST,
    I2C_ERR_UNKNOWN
} i2c_err_t;

// --- Modos de Pinout I2C ---
typedef enum {
    I2C_PINOUT_DEFAULT = 0, // SCL = PC2, SDA = PC1
    I2C_PINOUT_ALT1    = 1, // SCL = PD1, SDA = PD0
    I2C_PINOUT_ALT2    = 2  // SCL = PC5, SDA = PC6
} i2c_pinout_mode_t;


// --- Funções Públicas da Lib I2C ---

/**
 * @brief Inicializa o periférico I2C.
 * @param pinout_mode O modo de pinout I2C a ser usado (I2C_PINOUT_DEFAULT, I2C_PINOUT_ALT1, I2C_PINOUT_ALT2).
 * @return I2C_OK se sucesso, ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_init(const i2c_pinout_mode_t pinout_mode);

/**
 * @brief Envia um comando START no barramento I2C.
 * @return I2C_OK se sucesso, ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_start(void);

/**
 * @brief Envia um comando STOP no barramento I2C.
 * @return I2C_OK se sucesso, ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_stop(void);

/**
 * @brief Envia o endereço do dispositivo (7-bit) e o bit de R/W.
 * @param addr Endereço do dispositivo I2C (7-bit).
 * @param direction Direção da comunicação (I2C_TX para escrita, I2C_RX para leitura).
 * @return I2C_OK se sucesso (ACK recebido), ou I2C_ERR_ADDR_TX (NACK).
 */
i2c_err_t i2c_address_send(const uint8_t addr, const uint8_t direction);

/**
 * @brief Escreve um único byte no barramento I2C (após START e Endereço).
 * @param byte O byte a ser enviado.
 * @return I2C_OK se sucesso (ACK recebido), ou I2C_ERR_TX_BYTE (NACK).
 */
i2c_err_t i2c_write_byte(const uint8_t byte);

/**
 * @brief Lê um único byte do barramento I2C (após START e Endereço).
 * @param ack_bit I2C_ACK (para continuar lendo) ou I2C_NACK (para última leitura).
 * @param out_byte Ponteiro para armazenar o byte lido.
 * @return I2C_OK se sucesso, ou I2C_ERR_RX_BYTE.
 */
i2c_err_t i2c_read_byte(const uint8_t ack_bit, uint8_t *out_byte);

/**
 * @brief Escreve um buffer de bytes para um dispositivo I2C.
 * Encapsula start, address, write_bytes e stop.
 * @param addr Endereço I2C do dispositivo (7-bit).
 * @param data Ponteiro para os dados a serem escritos.
 * @param len Número de bytes a serem escritos.
 * @return I2C_OK se sucesso, ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_write_bytes(const uint8_t addr, const uint8_t *data, const uint8_t len);

/**
 * @brief Lê um buffer de bytes de um dispositivo I2C.
 * Encapsula start, address, read_bytes e stop.
 * @param addr Endereço I2C do dispositivo (7-bit).
 * @param data Ponteiro para onde os dados lidos serão armazenados.
 * @param len Número de bytes a serem lidos.
 * @return I2C_OK se sucesso, ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_read_bytes(const uint8_t addr, uint8_t *data, const uint8_t len);

/**
 * @brief Tenta fazer um ping (escrever endereço) em um dispositivo I2C.
 * @param addr Endereço I2C do dispositivo (7-bit).
 * @return I2C_OK se o dispositivo respondeu (ACK), ou um código de erro I2C_ERR_t.
 */
i2c_err_t i2c_ping(const uint8_t addr);

/**
 * @brief Scaneia o barramento I2C por dispositivos 7-bit.
 * @param callback Função de callback a ser chamada para cada endereço encontrado.
 */
void i2c_scan(void (*callback)(const uint8_t));

#ifdef __cplusplus
}
#endif

#endif // LIB_I2C_H
