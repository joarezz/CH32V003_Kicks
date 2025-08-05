/******************************************************************************
* lib_uart - A simple but full-featured library for UART on the CH32V003
*
*
*This library was adapted from the model described in the repository below.*
*Adapted by Joarez Fonseca
*
* See GitHub for more information:
* https://github.com/ADBeta/CH32V003_lib_uart
*
* 07 Apr 2025    Version 5.4
*
* Released under the MIT Licence
* Copyright ADBeta (c) 2024-2025
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
* USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/



#include "lib_uart.h"
#include "ch32fun.h"
#include <stddef.h>
#include <stdbool.h>

/*** Static Variables ********************************************************/
// Bool flag to prevent users from getting suck in a read/write loop if the
// UART was not able to init
static bool _uart_init_ok = false;

// UART RX Ring Buffer defined, gets set in uart_init()
static _uart_buffer_t _uart_rx_buffer = {NULL, 0,0,0};

// NOVAS VARIÁVEIS ESTÁTICAS PARA O BUFFER DE LINHA DO Serial_readString
#define SERIAL_LINE_BUFFER_SIZE 64 // Tamanho do buffer para acumular a linha
static char _serial_line_buffer[SERIAL_LINE_BUFFER_SIZE];
static size_t _serial_line_buffer_idx = 0;
static bool _serial_line_ready_flag = false; // Renomeado para evitar conflito/clareza



/*** IRQ Handler for UART ****************************************************/
/// @brief UART Receiver Interrupt handler - Puts the data received into the
/// UART Ring Buffer
/// @param None
/// @return None
void USART1_IRQHandler(void) __attribute__((interrupt));
void USART1_IRQHandler(void)
{
	
	if(USART1->STATR & USART_STATR_RXNE)
	{
		// Read from the DATAR Register to reset the flag
		uint8_t recv = (uint8_t)USART1->DATAR;

		// Calculate the next write position
		size_t next_head = (_uart_rx_buffer.head + 1) % _uart_rx_buffer.size;

		// If the next position is the same as the tail, either reject the new data
		// or overwrite old data
		if(next_head == _uart_rx_buffer.tail)
		{
			#ifdef RX_RING_BUFFER_OVERWRITE
				// Increment the tail position
				 _uart_rx_buffer.tail = (_uart_rx_buffer.tail + 1) % _uart_rx_buffer.size;
			#else
				// Reject any data that overfills the buffer
				return;
			#endif
		}

		// Add the received data to the current head position
		_uart_rx_buffer.buffer[_uart_rx_buffer.head] = recv;
		// Update the head position
		_uart_rx_buffer.head = next_head;
	}
}

/*** Initialiser *************************************************************/
uart_err_t uart_init( const uint8_t *rx_buffer_ptr,
					  const uint32_t rx_buffer_size,
					  const uart_config_t *conf     )
{
	// Make sure the input variables are valid.
	if(rx_buffer_ptr == NULL || rx_buffer_size == 0 || conf == NULL)
	{
		_uart_init_ok = false;
		return UART_NOT_INITIALIZED;
	}

	// Set up the RX Ring buffer Variables
	_uart_rx_buffer.buffer = (uint8_t *)rx_buffer_ptr;
	_uart_rx_buffer.size   = rx_buffer_size;
	_uart_rx_buffer.head   = 0;
	_uart_rx_buffer.tail   = 0;

	// Enable UART1 Clock
	RCC->APB2PCENR |= RCC_APB2Periph_USART1;
	// Enable the UART GPIO Port, and the Alternate Function IO Flag
	RCC->APB2PCENR |= UART_TXRX_PORT_RCC | RCC_APB2Periph_AFIO;

	// Reset, then set the AFIO Register depending on UART_PINOUT selected
	AFIO->PCFR1 &= UART_AFIO_RESET_MASK;
	AFIO->PCFR1 |= UART_AFIO_MASK;

	// TODO: RTS CTS pins if set
	// Set up the GPIO Pins for UART
	// TX 10MHz PP AF
	// RX INPUT_FLOATING
	UART_TXRX_PORT->CFGLR &= ~(0x0F << (4 * UART_PIN_TX));
	UART_TXRX_PORT->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * UART_PIN_TX);
	UART_TXRX_PORT->CFGLR &= ~(0x0F << (4 * UART_PIN_RX));
	UART_TXRX_PORT->CFGLR |= GPIO_CNF_IN_FLOATING << (4 * UART_PIN_RX);

	// Set CTLR1 Register (Enable RX & TX, set Word Length and Parity)
	USART1->CTLR1 = USART_Mode_Tx | USART_Mode_Rx | conf->wordlength | conf->parity;
	// Set CTLR2 Register (Stopbits)
	USART1->CTLR2 = conf->stopbits;
	// Set CTLR3 Register (Flow control)
	USART1->CTLR3 = (uint16_t)0x0000 | conf->flowctrl;

	// Set the Baudrate, assuming 48KHz
	USART1->BRR = conf->baudrate;

	// Enable the UART RXNE Interrupt
	USART1->CTLR1 |= USART_CTLR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);

	// Enable the UART
	USART1->CTLR1 |= USART_CTLR1_UE;

	// Set ok flag and return OK
	_uart_init_ok = true;
	return UART_OK;
}

uart_err_t Serial_begin(uint32_t baudrate)
{
    // Usaremos um buffer RX padrão para o Serial_begin
    static uint8_t default_rx_buffer[32]; // Buffer interno, tamanho fixo

    uart_config_t uart_conf = {
        .baudrate    = (uart_baudrate_t)UART_CALC_DIV(baudrate), // Converte int para enum
        .wordlength  = UART_WORDLENGTH_8,
        .parity      = UART_PARITY_NONE,
        .stopbits    = UART_STOPBITS_ONE,
        .flowctrl    = UART_FLOWCTRL_NONE,
    };

    return uart_init(default_rx_buffer, sizeof(default_rx_buffer), &uart_conf);
}


/*** Write *******************************************************************/
uart_err_t uart_write(const void *buffer, size_t size)
{
	if(!_uart_init_ok) return UART_NOT_INITIALIZED;
	if(buffer == NULL || size == 0) return UART_INVALID_ARGS;

	// Cast the input to a uint8_t
	const uint8_t *bytes = (const uint8_t *)buffer;
	while(size--)
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *bytes++;
	}

	return UART_OK;
}


uart_err_t uart_print(const char *string)
{
	if(!_uart_init_ok) return UART_NOT_INITIALIZED;
	if(string == NULL) return UART_INVALID_ARGS;

	while(*string != '\0')
	{
		// Wait for the current transmission to finish
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = *string++;
	}

	return UART_OK;
}


uart_err_t uart_println(const char *string)
{
	// Catches NULL or invalid string and not init
	uart_err_t ret_err = uart_print(string);

	// If input was valid & sent, send \r\n
	if(ret_err == UART_OK)
	{
		// Print the terminating characters
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = '\r';
		while(!(USART1->STATR & USART_FLAG_TC));
		USART1->DATAR = '\n';
	}

	return ret_err;
}


/** Read *********************************************************************/
size_t uart_read(uint8_t *buffer, size_t len)
{
	size_t bytes_read = 0;

	// Make sure the buffer passed and length are valid
	if(buffer != NULL && len != 0)
	{
		while(len--)
		{
			// If the buffer has no more data, return how many bytes were read
			if(_uart_rx_buffer.head == _uart_rx_buffer.tail) break;

			// Add the current tail byte to the buffer
			*buffer++ = _uart_rx_buffer.buffer[_uart_rx_buffer.tail];
			// Increment the ring buffer tail position
			_uart_rx_buffer.tail = (_uart_rx_buffer.tail + 1) % _uart_rx_buffer.size;

			// Increment the count of bytes
			bytes_read++;
		}
	}

	return bytes_read;
}

size_t Serial_read(uint8_t *buffer, size_t len)
{
    size_t bytes_read = 0;
    size_t original_len = len;

    if (buffer == NULL || len == 0) return 0;

    // Loop until buffer is full or no more data in ring buffer
    while (bytes_read < original_len)
    {
        // If the buffer has no more data, return how many bytes were read
        if (_uart_rx_buffer.head == _uart_rx_buffer.tail) break;

        uint8_t received_char = _uart_rx_buffer.buffer[_uart_rx_buffer.tail];

        // Increment the ring buffer tail position
        _uart_rx_buffer.tail = (_uart_rx_buffer.tail + 1) % _uart_rx_buffer.size;

        if (received_char == '\r')
        {
            // If we have space for '\r' and '\n'
            if (bytes_read + 1 < original_len) // +1 for '\n'
            {
                *buffer++ = '\r';
                *buffer++ = '\n';
                bytes_read += 2;
            }
            else if (bytes_read < original_len) // Only space for '\r'
            {
                *buffer++ = '\r';
                bytes_read++;
            }
            // If no space, just discard the character
        }
        else
        {
            *buffer++ = received_char;
            bytes_read++;
        }
    }
    return bytes_read;
}

size_t Serial_available(void)
{
    if (_uart_rx_buffer.head >= _uart_rx_buffer.tail)
    {
        return _uart_rx_buffer.head - _uart_rx_buffer.tail;
    }
    else
    {
        return _uart_rx_buffer.size - (_uart_rx_buffer.tail - _uart_rx_buffer.head);
    }
}

// NOVA IMPLEMENTAÇÃO DA Serial_readString
char* Serial_readString(void)
{
    // CÓDIGO CRÍTICO PARA EVITAR RETORNOS DE STRINGS VAZIAS:
    // Se uma linha foi preparada em uma chamada anterior, isso significa que
    // ela já foi "entregue". Agora, precisamos limpar o estado e sinalizar que
    // não há mais nenhuma linha pronta *nesta chamada*.
    if (_serial_line_ready_flag) {
        _serial_line_ready_flag = false; // Resetar a flag
        _serial_line_buffer_idx = 0;     // Limpar o índice
        _serial_line_buffer[0] = '\0';   // Limpar o conteúdo do buffer

        // Retornamos NULL para indicar que nenhuma string válida está disponível AGORA,
        // pois a linha anterior já foi processada e seu "slot" está limpo.
        return NULL; 
    }

    // Processa caracteres do buffer circular da UART
    while (Serial_available() > 0)
    {
        uint8_t received_char = _uart_rx_buffer.buffer[_uart_rx_buffer.tail];
        // Consome o caractere do buffer circular
        _uart_rx_buffer.tail = (_uart_rx_buffer.tail + 1) % _uart_rx_buffer.size;

        // Se for um terminador de linha ('\n' ou '\r')
        if (received_char == '\n' || received_char == '\r'|| received_char == 0x03)
        {
            // Consumir quaisquer outros delimitadores de linha imediatamente subsequentes.
            // Isso lida com sequências como "\r\n" ou múltiplas quebras de linha acidentais.
            while (Serial_available() > 0) {
                uint8_t next_char = _uart_rx_buffer.buffer[_uart_rx_buffer.tail];
                if (next_char == '\n' || next_char == '\r'|| next_char == 0x03) {
                    _uart_rx_buffer.tail = (_uart_rx_buffer.tail + 1) % _uart_rx_buffer.size;
                } else {
                    break; // Não é um delimitador, para de consumir
                }
            }
            
            // A linha completa foi recebida e está no _serial_line_buffer.
            _serial_line_buffer[_serial_line_buffer_idx] = '\0'; // Termina a string

            _serial_line_ready_flag = true; // Sinaliza que uma linha está pronta para ser lida.

            // Retorna o ponteiro para a string recém-preparada.
            // O reset do _serial_line_buffer_idx e a limpeza do buffer acontecerão
            // na PRÓXIMA chamada a Serial_readString (quando _serial_line_ready_flag for true).
            return _serial_line_buffer; 
        }
        else // Caractere normal (não é um delimitador de linha)
        {
            // Adiciona o caractere ao buffer de linha se houver espaço
            if (_serial_line_buffer_idx < (SERIAL_LINE_BUFFER_SIZE - 1)) // Deixa espaço para o terminador nulo
            {
                _serial_line_buffer[_serial_line_buffer_idx++] = received_char;
            }
            else
            {
                // O buffer de linha está cheio antes de um terminador ser encontrado.
                // Isso indica uma linha muito longa ou erro. Limpa o buffer e a flag.
                _serial_line_buffer_idx = 0;
                _serial_line_buffer[0] = '\0';
                _serial_line_ready_flag = false; 
                // Opcional: Serial_println("Erro: Linha muito longa!"); para depuração.
            }
        }
    }
    
    // Nenhuma linha completa foi detectada ou está disponível no momento.
    return NULL; 
}
