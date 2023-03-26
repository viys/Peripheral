/*
 * UartRingbuffer.h
 *
 *  Created on: 1-June-2021
 *      Author: biubiu
 */


#ifndef UARTRINGBUFFER_H_
#define UARTRINGBUFFER_H_


#include "stm32f1xx_hal.h"

/* change the size of the buffer */
#define UART_BUFFER_SIZE 10240

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;


/* reads the data in the rx_buffer and increment the tail count in rx_buffer of the given UART */
int uart_read(UART_HandleTypeDef *uart);

/* writes the data to the tx_buffer and increment the head count in tx_buffer */
void uart_write(int c, UART_HandleTypeDef *uart);

/* function to send the string to the uart */
void uart_sendstring(const char *s, UART_HandleTypeDef *uart);

/* Print a number with any base
 * base can be 10, 8 etc*/
void uart_printbase (long n, uint8_t base, UART_HandleTypeDef *uart);

/* Initialize the ring buffer */
void ring_buffer_init(void);

void uart_flush (UART_HandleTypeDef *uart);

/* checks if the data is available to read in the rx_buffer of the uart */
int is_data_available(UART_HandleTypeDef *uart);

/* Look for a particular string in the given buffer
 * @return 1, if the string is found and -1 if not found
 * @USAGE:: if (ring_buffer_look_for ("some string", buffer)) do something
 */
int ring_buffer_look_for (char *str, char *buffertolookinto);

void get_data_form_ring_buffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto);

/* Peek for the data in the Rx Bffer without incrementing the tail count
* Returns the character
* USAGE: if (uart_peek () == 'M') do something
*/
int uart_peek(UART_HandleTypeDef *uart);


/* Copy the data from the Rx buffer into the buffer, Upto and including the entered string
* This copying will take place in the blocking mode, so you won't be able to perform any other operations
* Returns 1 on success and -1 otherwise
* USAGE: while (!(Copy_Upto ("some string", buffer, uart)));
*/
int copy_upto (char *string, char *buffertocopyinto, UART_HandleTypeDef *uart);


/* Copies the entered number of characters (blocking mode) from the Rx buffer into the buffer, after some particular string is detected
* Returns 1 on success and -1 otherwise
* USAGE: while (!(get_after ("some string", 6, buffer, uart)));
*/
int get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart);

/* Wait until a paricular string is detected in the Rx Buffer
* Return 1 on success and -1 otherwise
* USAGE: while (!(wait_for("some string", uart)));
*/
int wait_for (char *string, UART_HandleTypeDef *uart);

/* Wait until a paricular string is detected in the Rx Buffer, wait time (s)
* Return 1 on success and -1 otherwise
* USAGE: while (!(wait_for(1000, "some string", uart)));
*/
int wait_for_time_out (uint16_t wait_time, char *string, UART_HandleTypeDef *uart);

/* the ISR for the uart. put it in the IRQ handler */
void uart_isr (UART_HandleTypeDef *huart);
void uart1_isr (UART_HandleTypeDef *huart);
void uart3_isr (UART_HandleTypeDef *huart);

/*** Depreciated For now. This is not needed, try using other functions to meet the requirement ***/
/* get the position of the given string within the incoming data.
 * It returns the position, where the string ends
 */
/* get the position of the given string in the given UART's incoming data.
 * It returns the position, where the string ends */
//int16_t Get_position (char *string, UART_HandleTypeDef *uart);


#endif /* UARTRINGBUFFER_H_ */
