#include "uart_ring_buffer_multi.h"
#include "esp8266_smartconfig_hal.h"
#include "esp8266_handler_hal.h"
#include "stdlib.h"

//home-assistant: 100ask 100ask.org

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define PC_UART 	&huart1
#define WIFI_UART 	&huart3


static void esp8266_smartchonfig_Start(void);



void esp8266_smartconfig_init(void)
{
	uart_sendstring("SmartConfig mode...\r\n", PC_UART);
	HAL_Delay(1000);
	at_cmd_handler("AT+RST\r\n", "OK\r\n", 5000, 10);
	uart_flush(WIFI_UART);
	uart_sendstring("[ok]RESETTING...\r\n", PC_UART);

	if (wait_for_time_out(10000, "WIFI GOT IP", WIFI_UART) == 1)
	{
		uart_sendstring("[ok]WIFI GOT IP successful!\n\n", PC_UART);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		esp8266_smartchonfig_Start();
	}
}


static void esp8266_smartchonfig_Start(void)
{
	char *ipaddr = NULL;
	uart_sendstring("Enter SmartConfig mode.\r\n", PC_UART);
	/********* AT **********/
	at_cmd_handler("AT\r\n", "OK\r\n", 5000, 200);

	/********* AT+CWMODE=1 **********/
	at_cmd_handler("AT+CWMODE_CUR=1\r\n", "OK\r\n", 5000, 200);
	//at_cmd_handler("AT+CWMODE_DEF=1\r\n", "OK\r\n", 5000, 200); // 设置 Wi-Fi 模式 (STA/AP/STA+AP)，保存到 Flash

	/********* AT+CWSTARTSMART **********/
	//at_cmd_handler("AT+CWSTARTSMART=3\r\n", "OK\r\n", 5000, 200); // 1:ESP-TOUCH, 2:AirKiss, 3:ESP-TOUCH+AirKiss
	 
	at_cmd_handler("AT+CWSTARTSMART\r\n", "OK\r\n", 5000, 200); // 不指定，默认使用：ESP-TOUCH+AirKiss
	while(1)
	{
		if (wait_for_time_out(10000, " ", WIFI_UART) == 1)
		{
			uart_sendstring("[ok]WIFI GOT IP successful!\n\n", PC_UART);
			
			/********* AT+CWSTOPSMART **********/
			at_cmd_handler("AT+CWSTOPSMART\r\n", "OK\r\n", 5000, 200); // 无论 SmartConfig 成功与否，都要尽快调用 AT+CWSTOPSMART 释放快连占用的内存。
			
			/********* AT+CIFSR **********/
			ipaddr = get_dev_ip(5000, 1);
			free(ipaddr);
			HAL_Delay(200);

			break;
		}
	}
}



