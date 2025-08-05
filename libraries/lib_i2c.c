
//JOAREZ DE OLIVEIRA DA FONSECA
//KICKS TECNOLOGIA -> SE INSCREVAM NO CANAL E NOS AJUDE A CRESCER!!!!!
//FRAMEWORK PARA A BIBLIOTECA : CH32VFUN
//22/06/2025
//https://youtu.be/l0zwVACMMVA

#include "lib_i2c.h"
#include "timers_utils.h" // Adicionado: para ter acesso à SystemCoreClock

// For debuging with Serial_println (descomente se estiver usando lib_uart para depuração)
// #include "lib_uart.h"
// #include <stdio.h> // For sprintf

// I2C Hardware Define: I2C1_SCL = PB6, I2C1_SDA = PB7.
// CH32V003Fun defines: SCL=PC2, SDA=PC1 for I2C
// Alt 1: SCL=PD1, SDA=PD0
// Alt 2: SCL=PC5, SDA=PC6

// --- Define I2C Timeout ---
#define I2C_TIMEOUT_COUNT 0x0000FFFF // Timeout para loops de espera

// Macro para esperar por uma condição com timeout
#define I2C_TIMEOUT_WAIT_FOR(condition, err_code) \
    do { \
        uint32_t timeout_counter = I2C_TIMEOUT_COUNT; \
        while ((condition) && (timeout_counter-- > 0)); \
        if (timeout_counter == 0) { \
            i2c_ret = I2C_ERR_TIMEOUT; \
            return i2c_ret; \
        } \
    } while(0)

// --- Variáveis Estáticas Internas ---
static uint32_t _i2c_scl_pin;
static uint32_t _i2c_sda_pin;

// --- Definições Internas para Funções I2C (ACK/NACK, TX/RX) ---
//#define I2C_ACK             0x00
//#define I2C_NACK            0x01
//#define I2C_TX              0x00
//#define I2C_RX              0x01


// --- Funções Privadas da Lib I2C ---

/**
 * @brief Reseta o periférico I2C.
 */
static void i2c_reset(void) {
    I2C1->CTLR1 &= ~I2C_CTLR1_PE; // Disable I2C peripheral
    I2C1->CTLR1 |= I2C_CTLR1_SWRST; // Software reset
    Delay_Us(10); // Small delay for reset to take effect
    I2C1->CTLR1 &= ~I2C_CTLR1_SWRST; // Clear software reset
    I2C1->CTLR1 |= I2C_CTLR1_PE; // Enable I2C peripheral
}

/**
 * @brief Realiza o tratamento de erro I2C (limpa flags).
 */
static void i2c_error(void) {
    I2C1->STAR1 = 0; // Clear all status flags
    I2C1->CTLR1 &= ~I2C_CTLR1_PE; // Disable I2C peripheral
    I2C1->CTLR1 |= I2C_CTLR1_PE; // Enable I2C peripheral
}


// --- Funções Públicas da Lib I2C ---

i2c_err_t i2c_init(const i2c_pinout_mode_t pinout_mode) {
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1; // Enable I2C1 clock
    
    // Configure GPIOs based on selected pinout mode
    if (pinout_mode == I2C_PINOUT_DEFAULT) {
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOC; // Enable GPIOC clock
        _i2c_scl_pin = PC2;
        _i2c_sda_pin = PC1;
    } else if (pinout_mode == I2C_PINOUT_ALT1) {
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOD; // Enable GPIOD clock
        _i2c_scl_pin = PD1;
        _i2c_sda_pin = PD0;
    } else if (pinout_mode == I2C_PINOUT_ALT2) {
        RCC->APB2PCENR |= RCC_APB2Periph_GPIOC; // Enable GPIOC clock
        _i2c_scl_pin = PC5;
        _i2c_sda_pin = PC6;
    } else {
        return I2C_ERR_INIT; // Invalid pinout mode
    }

    // Configure SCL and SDA pins for AF_OD (Alternate Function Open-Drain)
    funPinMode(_i2c_scl_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_OD_AF);
    funPinMode(_i2c_sda_pin, GPIO_Speed_50MHz | GPIO_CNF_OUT_OD_AF);

    // I2C Reset (Software) - useful if I2C is stuck
    i2c_reset();

    // I2C Configuration
    // CTRLR2: APB1 clock frequency in MHz for I2C.
    // If SystemCoreClock is 48MHz, PCLK1 (APB1) is 24MHz (SystemCoreClock / 2).
    I2C1->CTLR2 = (SystemCoreClock / 1000000 / 2); // PCLK1 frequency in MHz.

    // Configure Clock Control Register (CKCFGR) for 100kHz standard mode
    // Standard Mode (Sm) calculation: Thigh = Tlow = CCR * T_PCLK1
    // T_PCLK1 = 1 / PCLK1 (e.g., 1 / 24,000,000 Hz = 41.67 ns)
    // For 100kHz: T_I2C = 10 us. T_I2C = 2 * Thigh = 2 * Tlow
    // Thigh = Tlow = 5 us = 5000 ns
    // CKCFGR = Thigh / T_PCLK1 = 5000 ns / 41.67 ns = 120.0
    // So, CKCFGR = 120 (0x78)
    I2C1->CKCFGR = 0x78; // 100kHz standard mode (using CKCFGR instead of CKCTLR)
    
    // O CH32V003 não possui o registrador RTR (Rise Time Register)
    // I2C1->RTR = ...; // REMOVIDO: Este registrador não existe no CH32V003

    I2C1->CTLR1 |= I2C_CTLR1_PE; // Enable I2C peripheral

    return I2C_OK;
}

i2c_err_t i2c_start(void) {
    i2c_err_t i2c_ret = I2C_OK;

    // Wait if bus is busy
    I2C_TIMEOUT_WAIT_FOR(I2C1->STAR2 & I2C_STAR2_BUSY, i2c_ret);
    if(i2c_ret != I2C_OK) return I2C_ERR_START;

    I2C1->CTLR1 |= I2C_CTLR1_START; // Generate START condition

    // Wait for SB (Start Bit) flag
    I2C_TIMEOUT_WAIT_FOR(!(I2C1->STAR1 & I2C_STAR1_SB), i2c_ret);
    if(i2c_ret != I2C_OK) return I2C_ERR_START;

    (void)I2C1->STAR1; // Clear SB by reading STAR1 then STAR2
    return I2C_OK;
}

i2c_err_t i2c_stop(void) {
    i2c_err_t i2c_ret = I2C_OK;

    I2C1->CTLR1 |= I2C_CTLR1_STOP; // Generate STOP condition

    // Wait for STOPF (Stop Failure) or BUSY flag to clear (not really STOPF, but BUSY)
    I2C_TIMEOUT_WAIT_FOR(I2C1->STAR2 & I2C_STAR2_BUSY, i2c_ret);
    if(i2c_ret != I2C_OK) {
        // This might not be an actual error for STOP, but rather bus still busy briefly.
        // For robustness, reset I2C if stop seems to fail permanently.
        // i2c_error(); // Optional: uncomment for aggressive error handling
        return I2C_ERR_STOP;
    }
    return I2C_OK;
}

i2c_err_t i2c_address_send(const uint8_t addr, const uint8_t direction) {
    i2c_err_t i2c_ret = I2C_OK;
    
    // Send 7-bit address + R/W bit
    I2C1->DATAR = (addr << 1) | direction;

    // Wait for ADDR (Address Sent) flag
    I2C_TIMEOUT_WAIT_FOR(!(I2C1->STAR1 & I2C_STAR1_ADDR), i2c_ret);
    if(i2c_ret != I2C_OK) {
        if(I2C1->STAR1 & I2C_STAR1_AF) i2c_error(); // Clear AF flag if NACK
        return I2C_ERR_ADDR_TX; // Return specific error for address NACK/timeout
    }

    (void)I2C1->STAR1; // Clear ADDR by reading STAR1 then STAR2
    (void)I2C1->STAR2;
    return I2C_OK;
}

i2c_err_t i2c_write_byte(const uint8_t byte) {
    i2c_err_t i2c_ret = I2C_OK;
    I2C1->DATAR = byte; // Write data byte

    // Wait for TXE (Transmit Data Register Empty) flag
    I2C_TIMEOUT_WAIT_FOR(!(I2C1->STAR1 & I2C_STAR1_TXE), i2c_ret);
    if(i2c_ret != I2C_OK) return I2C_ERR_TX_BYTE;

    // Wait for BTF (Byte Transfer Finished) flag
    I2C_TIMEOUT_WAIT_FOR(!(I2C1->STAR1 & I2C_STAR1_BTF), i2c_ret);
    if(i2c_ret != I2C_OK) return I2C_ERR_TX_BYTE;

    // Check for NACK after byte transmission
    if(I2C1->STAR1 & I2C_STAR1_AF) {
        i2c_error(); // Clear AF flag
        return I2C_ERR_TX_BYTE; // Return specific error for byte NACK
    }

    return I2C_OK;
}

i2c_err_t i2c_read_byte(const uint8_t ack_bit, uint8_t *out_byte) {
    i2c_err_t i2c_ret = I2C_OK;

    if (ack_bit == I2C_ACK) {
        I2C1->CTLR1 |= I2C_CTLR1_ACK; // Enable ACK
    } else {
        I2C1->CTLR1 &= ~I2C_CTLR1_ACK; // Disable ACK (send NACK for last byte)
    }

    // Wait for RXNE (Receive Data Register Not Empty) flag
    I2C_TIMEOUT_WAIT_FOR(!(I2C1->STAR1 & I2C_STAR1_RXNE), i2c_ret);
    if(i2c_ret != I2C_OK) return I2C_ERR_RX_BYTE;

    *out_byte = I2C1->DATAR; // Read data byte

    if (ack_bit == I2C_NACK) {
        i2c_stop(); // Generate STOP condition after last byte read
    }

    return I2C_OK;
}

i2c_err_t i2c_write_bytes(const uint8_t addr, const uint8_t *data, const uint8_t len) {
    i2c_err_t err;
    if ((err = i2c_start()) != I2C_OK) return err;
    if ((err = i2c_address_send(addr, I2C_TX)) != I2C_OK) {
        i2c_stop(); // Ensure stop on address NACK
        return err;
    }
    for (uint8_t i = 0; i < len; i++) {
        if ((err = i2c_write_byte(data[i])) != I2C_OK) {
            i2c_stop(); // Ensure stop on data NACK/timeout
            return err;
        }
    }
    i2c_stop();
    return I2C_OK;
}

i2c_err_t i2c_read_bytes(const uint8_t addr, uint8_t *data, const uint8_t len) {
    i2c_err_t err;
    if ((err = i2c_start()) != I2C_OK) return err;
    if ((err = i2c_address_send(addr, I2C_RX)) != I2C_OK) {
        i2c_stop(); // Ensure stop on address NACK
        return err;
    }
    for (uint8_t i = 0; i < len; i++) {
        uint8_t ack_bit = (i == len - 1) ? I2C_NACK : I2C_ACK; // NACK for the last byte
        if ((err = i2c_read_byte(ack_bit, &data[i])) != I2C_OK) {
            i2c_stop(); // Ensure stop on read error
            return err;
        }
    }
    // i2c_read_byte handles stop for the last byte when NACK is sent
    return I2C_OK;
}


i2c_err_t i2c_ping(const uint8_t addr) {
    i2c_err_t err;
    if ((err = i2c_start()) != I2C_OK) return err;
    err = i2c_address_send(addr, I2C_TX);
    i2c_stop(); // Always send stop after ping
    return err; // Will return I2C_OK if ACK was received, otherwise I2C_ERR_ADDR_TX
}

void i2c_scan(void (*callback)(const uint8_t)) {
    // char serial_buffer[30]; // Warning: unused variable 'serial_buffer' if Serial_println is commented
    // Serial_println("I2C Scanner:"); // Uncomment for debug

    for (uint8_t addr = 1; addr < 127; addr++) { // 7-bit addresses 0x01 to 0x7E
        if (i2c_ping(addr) == I2C_OK) {
            // sprintf(serial_buffer, "Found device at 0x%02X", addr); // Uncomment for debug
            // Serial_println(serial_buffer);
            if (callback) {
                callback(addr);
            }
        }
        // Delay_Ms(1); // Small delay between pings to prevent bus locking
    }
    // Serial_println("Scan complete."); // Uncomment for debug
}
