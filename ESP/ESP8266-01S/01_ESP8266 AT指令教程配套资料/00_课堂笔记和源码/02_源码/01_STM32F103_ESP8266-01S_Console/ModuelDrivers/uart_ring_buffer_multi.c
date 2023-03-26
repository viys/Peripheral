/*
 * UartRingbuffer.c
 *
 *  Created on: 1-June-2021
 *      Author: biubiu
 */

#include <string.h>
#include "main.h"
#include "uart_ring_buffer_multi.h"

/*  Define the device uart and pc uart below according to your setup  */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define BWW_UART1 &huart1
#define BWW_UART3 &huart3

/* put the following in the ISR 

extern void Uart_isr (UART_HandleTypeDef *huart);

*/

/**************** =====================================>>>>>>>>>>>> NO chnages after this **********************/


ring_buffer rx_buffer1 = { { 0 }, 0, 0};
ring_buffer tx_buffer1 = { { 0 }, 0, 0};
ring_buffer rx_buffer2 = { { 0 }, 0, 0};
ring_buffer tx_buffer2 = { { 0 }, 0, 0};

ring_buffer *_rx_buffer1;
ring_buffer *_tx_buffer1;
ring_buffer *_rx_buffer2;
ring_buffer *_tx_buffer2;

void store_char (unsigned char c, ring_buffer *buffer);


void ring_buffer_init(void)
{
  _rx_buffer1 = &rx_buffer1;
  _tx_buffer1 = &tx_buffer1;
  _rx_buffer2 = &rx_buffer2;
  _tx_buffer2 = &tx_buffer2;

  /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_ENABLE_IT(BWW_UART1, UART_IT_ERR);
  __HAL_UART_ENABLE_IT(BWW_UART3, UART_IT_ERR);

  /* Enable the UART Data Register not empty Interrupt */
  __HAL_UART_ENABLE_IT(BWW_UART1, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(BWW_UART3, UART_IT_RXNE);
}

void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % UART_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if(i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}

/* Look for a particular string in the given buffer
 * @return 1, if the string is found and -1 if not found
 * @USAGE:: if (ring_buffer_look_for ("some string", buffer)) do something
 */
int ring_buffer_look_for (char *str, char *buffertolookinto)
{
	int stringlength = strlen (str);
	int bufferlength = strlen (buffertolookinto);
	int so_far = 0;
	int indx = 0;
repeat:
	while (str[so_far] != buffertolookinto[indx]) indx++;
	if (str[so_far] == buffertolookinto[indx]){
	while (str[so_far] == buffertolookinto[indx])
	{
		so_far++;
		indx++;
	}
	}

	else
	{
		so_far =0;
		if (indx >= bufferlength) return -1;
		goto repeat;
	}

	if (so_far == stringlength) return 1;
	else return -1;
}

void get_data_form_ring_buffer (char *startString, char *endString, char *buffertocopyfrom, char *buffertocopyinto)
{
	int startStringLength = strlen (startString);
	int endStringLength   = strlen (endString);
	int so_far = 0;
	int indx = 0;
	int startposition = 0;
	int endposition = 0;

repeat1:
	while (startString[so_far] != buffertocopyfrom[indx]) indx++;
	if (startString[so_far] == buffertocopyfrom[indx])
	{
		while (startString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == startStringLength) startposition = indx;
	else
	{
		so_far =0;
		goto repeat1;
	}

	so_far = 0;

repeat2:
	while (endString[so_far] != buffertocopyfrom[indx]) indx++;
	if (endString[so_far] == buffertocopyfrom[indx])
	{
		while (endString[so_far] == buffertocopyfrom[indx])
		{
			so_far++;
			indx++;
		}
	}

	if (so_far == endStringLength) endposition = indx-endStringLength;
	else
	{
		so_far =0;
		goto repeat2;
	}

	so_far = 0;
	indx=0;

	for (int i=startposition; i<endposition; i++)
	{
		buffertocopyinto[indx] = buffertocopyfrom[i];
		indx++;
	}
}


void uart_flush (UART_HandleTypeDef *uart)
{
	if (uart == BWW_UART1)
	{
		memset(_rx_buffer1->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer1->head = 0;
		_rx_buffer1->tail = 0;
	}
	if (uart == BWW_UART3)
	{
		memset(_rx_buffer2->buffer,'\0', UART_BUFFER_SIZE);
		_rx_buffer2->head = 0;
		_rx_buffer2->tail = 0;
	}
}


int uart_peek(UART_HandleTypeDef *uart)
{
	if (uart == BWW_UART1)
	{
		  if(_rx_buffer1->head == _rx_buffer1->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    return _rx_buffer1->buffer[_rx_buffer1->tail];
		  }
	}

	else if (uart == BWW_UART3)
	{
		  if(_rx_buffer2->head == _rx_buffer2->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    return _rx_buffer2->buffer[_rx_buffer2->tail];
		  }
	}

	return -1;
}

int uart_read(UART_HandleTypeDef *uart)
{
	if (uart == BWW_UART1)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer1->head == _rx_buffer1->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
		    _rx_buffer1->tail = (unsigned int)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}

	else if (uart == BWW_UART3)
	{
		  // if the head isn't ahead of the tail, we don't have any characters
		  if(_rx_buffer2->head == _rx_buffer2->tail)
		  {
		    return -1;
		  }
		  else
		  {
		    unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
		    _rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
		    return c;
		  }
	}

	else return -1;
}

void uart_write(int c, UART_HandleTypeDef *uart)
{
	if (c>=0)
	{
		if (uart == BWW_UART1){
		int i = (_tx_buffer1->head + 1) % UART_BUFFER_SIZE;

		// If the output buffer is full, there's nothing for it other than to
		// wait for the interrupt handler to empty it a bit
		// ???: return 0 here instead?
		while (i == _tx_buffer1->tail);

		_tx_buffer1->buffer[_tx_buffer1->head] = (uint8_t)c;
		_tx_buffer1->head = i;

		__HAL_UART_ENABLE_IT(BWW_UART1, UART_IT_TXE); // Enable UART transmission interrupt
		}

		else if (uart == BWW_UART3){
			int i = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

			// If the output buffer is full, there's nothing for it other than to
			// wait for the interrupt handler to empty it a bit
			// ???: return 0 here instead?
			while (i == _tx_buffer2->tail);

			_tx_buffer2->buffer[_tx_buffer2->head] = (uint8_t)c;
			_tx_buffer2->head = i;

			__HAL_UART_ENABLE_IT(BWW_UART3, UART_IT_TXE); // Enable UART transmission interrupt
			}
	}
}

int is_data_available(UART_HandleTypeDef *uart)
{
	if (uart == BWW_UART1) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;
	else if (uart == BWW_UART3) return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
	return -1;
}


/* Copies the entered number of characters (blocking mode) from the Rx buffer into the buffer, after some particular string is detected
* Returns 1 on success and -1 otherwise
* USAGE: while (!(get_after ("some string", 6, buffer, uart)));
*/
int get_after (char *string, uint8_t numberofchars, char *buffertosave, UART_HandleTypeDef *uart)
{

	while (wait_for(string, uart) != 1);
	for (int indx=0; indx < numberofchars; indx++)
	{
		while (!(is_data_available(uart)));
		buffertosave[indx] = uart_read(uart);
	}
	return 1;
}

/* function to send the string to the uart */
void uart_sendstring (const char *s, UART_HandleTypeDef *uart)
{
	while(*s!='\0') uart_write(*s++, uart);
}

void uart_printbase (long n, uint8_t base, UART_HandleTypeDef *uart)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  while(*s) uart_write(*s++, uart);
}


/* Copy the data from the Rx buffer into the buffer, Upto and including the entered string
* This copying will take place in the blocking mode, so you won't be able to perform any other operations
* Returns 1 on success and -1 otherwise
* USAGE: while (!(Copy_Upto ("some string", buffer, uart)));
*/
int copy_upto (char *string, char *buffertocopyinto, UART_HandleTypeDef *uart)
{
	int so_far =0;
	int len = strlen (string);
	int indx = 0;

again:
	while (!is_data_available(uart));
	while (uart_peek(uart) != string[so_far])
	{
		buffertocopyinto[indx] = _rx_buffer2->buffer[_rx_buffer2->tail];
		_rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
		indx++;
		while (!is_data_available(uart));

	}
	while (uart_peek(uart) == string [so_far])
	{
		so_far++;
		buffertocopyinto[indx++] = uart_read(uart);
		if (so_far == len) return 1;
		while (!is_data_available(uart));
	}

	if (so_far != len)
	{
		so_far = 0;
		goto again;
	}

	if (so_far == len) return 1;
	else return -1;

}

/* Wait until a paricular string is detected in the Rx Buffer
* Return 1 on success and -1 otherwise
* USAGE: while (!(wait_for("some string", uart)));
*/
int wait_for_time_out (uint16_t wait_time, char *string, UART_HandleTypeDef *uart)
{
	int so_far = 0;
	int len = strlen (string);

	receive_time = wait_time;
again_device:
	if(receive_time == 0)
	{
		uart_sendstring("!!!TIME OUT!!!\r\n", BWW_UART1);
		return -1;
	}
	if (is_data_available(uart) == (-1))
		goto again_device;
	if (uart_peek(uart) != string[so_far])
	{
		_rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE ;
		goto again_device;

	}
	while (uart_peek(uart) == string [so_far])
	{
		so_far++;
		uart_read(uart);
		if (so_far == len) return 1;
		while (!is_data_available(uart));
	}

	if (so_far != len)
	{
		so_far = 0;
		goto again_device;
	}

	if (so_far == len) return 1;
	else return -1;
}

/* Wait until a paricular string is detected in the Rx Buffer
* Return 1 on success and -1 otherwise
* USAGE: while (!(wait_for("some string", uart)));
*/
int wait_for (char *string, UART_HandleTypeDef *uart)
{
	int so_far = 0;
	int len = strlen (string);

again_device:
	while(!is_data_available(uart));
	if (uart_peek(uart) != string[so_far])
	{
		_rx_buffer2->tail = (unsigned int)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE ;
		goto again_device;

	}
	while (uart_peek(uart) == string [so_far])
	{
		so_far++;
		uart_read(uart);
		if (so_far == len) return 1;
		while (!is_data_available(uart));
	}

	if (so_far != len)
	{
		so_far = 0;
		goto again_device;
	}

	if (so_far == len) return 1;
	else return -1;
}

#if 1
void Uart_isr(UART_HandleTypeDef *huart)
{
	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
	  uint32_t cr3its     = READ_REG(huart->Instance->CR3);

    /* if DR is not empty and the Rx Int is enabled */
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	  //if (((isrflags & USART_SR_RXNE) != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || (cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET))
    {
				 /******************
				*  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
				*          error) and IDLE (Idle line detected) flags are cleared by software
				*          sequence: a read operation to USART_SR register followed by a read
				*          operation to USART_DR register.
				* @note   RXNE flag can be also cleared by a read to the USART_DR register.
				* @note   TC flag can be also cleared by software sequence: a read operation to
				*          USART_SR register followed by a write operation to USART_DR register.
				* @note   TXE flag is cleared only by a write to the USART_DR register.
				 *********************/
				READ_REG(huart->Instance->SR);
//huart->Instance->SR;                       /* Read status register */
        unsigned char c = huart->Instance->DR;     /* Read data register */
        if (huart == BWW_UART1)
        {
        	  store_char (c, _rx_buffer1);  // store data in buffer
        }
        else if (huart == BWW_UART3)
        {
           	store_char (c, _rx_buffer2);  // store data in buffer
        }

        //return;
    }
    /* If interrupt is caused due to Transmit Data Register Empty */
    else if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
    	if (huart == BWW_UART1)
    	{
					if(tx_buffer1.head == tx_buffer1.tail)
					{
							// Buffer empty, so disable interrupts
							__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

					}

					else
					{
							// There is more data in the output buffer. Send the next byte
							unsigned char c = tx_buffer1.buffer[tx_buffer1.tail];
							tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

							/******************
							* @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
							*         error) and IDLE (Idle line detected) flags are cleared by software
							*         sequence: a read operation to USART_SR register followed by a read
							*         operation to USART_DR register.
							* @note   RXNE flag can be also cleared by a read to the USART_DR register.
							* @note   TC flag can be also cleared by software sequence: a read operation to
							*         USART_SR register followed by a write operation to USART_DR register.
							* @note   TXE flag is cleared only by a write to the USART_DR register.

							*********************/

							huart->Instance->SR;
							huart->Instance->DR = c;
					}
				}
				else if (huart == BWW_UART3)
				{
						if(tx_buffer2.head == tx_buffer2.tail)
						{
							// Buffer empty, so disable interrupts
							__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

						}

						else
						{
							// There is more data in the output buffer. Send the next byte
							unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
							tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;

							/******************
							* @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
							*         error) and IDLE (Idle line detected) flags are cleared by software
							*         sequence: a read operation to USART_SR register followed by a read
							*         operation to USART_DR register.
							* @note   RXNE flag can be also cleared by a read to the USART_DR register.
							* @note   TC flag can be also cleared by software sequence: a read operation to
							*         USART_SR register followed by a write operation to USART_DR register.
							* @note   TXE flag is cleared only by a write to the USART_DR register.

							*********************/
							huart->Instance->SR;
							huart->Instance->DR = c;
						}
					}
			//return;
    }
}
#endif
#if 0
void Uart_isr(UART_HandleTypeDef *huart)
{
	  uint32_t isrflags   = READ_REG(huart->Instance->SR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
	  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
		uint32_t errorflags = 0x00U;

		/* If no error occurs */
    errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
		if (errorflags == RESET)
		{
			/* UART in mode Receiver -------------------------------------------------*/
			if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
			{
				READ_REG(huart->Instance->SR);
				unsigned char c = huart->Instance->DR;     /* Read data register */
        if (huart == BWW_UART1)
        {
						store_char (c, _rx_buffer1);  // store data in buffer
        }
        else if (huart == BWW_UART3)
        {
           	store_char (c, _rx_buffer2);  // store data in buffer
        }
				//return;
			}
		}
    /* If interrupt is caused due to Transmit Data Register Empty */
    else if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
    {
    	if (huart == BWW_UART1)
    	{
					if(tx_buffer1.head == tx_buffer1.tail)
					{
							// Buffer empty, so disable interrupts
							__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
					}

					else
					{
							// There is more data in the output buffer. Send the next byte
							unsigned char c = tx_buffer1.buffer[tx_buffer1.tail];
							tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

							/******************
							* @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
							*         error) and IDLE (Idle line detected) flags are cleared by software
							*         sequence: a read operation to USART_SR register followed by a read
							*         operation to USART_DR register.
							* @note   RXNE flag can be also cleared by a read to the USART_DR register.
							* @note   TC flag can be also cleared by software sequence: a read operation to
							*         USART_SR register followed by a write operation to USART_DR register.
							* @note   TXE flag is cleared only by a write to the USART_DR register.

							*********************/

							huart->Instance->SR;
							huart->Instance->DR = c;
					}
				}
				else if (huart == BWW_UART3)
				{
						if(tx_buffer2.head == tx_buffer2.tail)
						{
							// Buffer empty, so disable interrupts
							__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
						}

						else
						{
							// There is more data in the output buffer. Send the next byte
							unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
							tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;

							/******************
							* @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
							*         error) and IDLE (Idle line detected) flags are cleared by software
							*         sequence: a read operation to USART_SR register followed by a read
							*         operation to USART_DR register.
							* @note   RXNE flag can be also cleared by a read to the USART_DR register.
							* @note   TC flag can be also cleared by software sequence: a read operation to
							*         USART_SR register followed by a write operation to USART_DR register.
							* @note   TXE flag is cleared only by a write to the USART_DR register.

							*********************/
							huart->Instance->SR;
							huart->Instance->DR = c;
						}
					}
			//return;
    }
}
#endif

#if 0
void Uart_isr(UART_HandleTypeDef *huart)
{
		unsigned char rx_buffer;
		unsigned char tx_buffer;
    if (huart == BWW_UART1)
		{
				
        //HAL_UART_Receive_IT(huart, &c, 1);
				HAL_UART_Receive(huart, &rx_buffer, 1, 1);
				store_char (rx_buffer, _rx_buffer1);  // store data in buffer
				if(tx_buffer1.head == tx_buffer1.tail)
				{
						// Buffer empty, so disable interrupts
						__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
						return;
				}
				else
				{
						// There is more data in the output buffer. Send the next byte
						tx_buffer = tx_buffer1.buffer[tx_buffer1.tail];
						tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;
						//HAL_UART_Transmit_IT(huart, &tx_buffer, 1);
						HAL_UART_Transmit(&huart1, &tx_buffer, 1, 1);
				}
		}
		else if (huart == BWW_UART3)
		{
        //HAL_UART_Receive_IT(huart, &rx_buffer, 1);
				HAL_UART_Receive(huart, &rx_buffer, 1, 1);
				store_char (rx_buffer, _rx_buffer2);  // store data in buffer
				if(tx_buffer2.head == tx_buffer2.tail)
				{
						// Buffer empty, so disable interrupts
						__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
					  return;
				}
				else
				{
						// There is more data in the output buffer. Send the next byte
						tx_buffer = tx_buffer2.buffer[tx_buffer2.tail];
						tx_buffer2.tail = (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;
						//HAL_UART_Transmit_IT(huart, &tx_buffer, 1);
						HAL_UART_Transmit(&huart1, &tx_buffer, 1, 1);
				}
		}
		
}
#endif

