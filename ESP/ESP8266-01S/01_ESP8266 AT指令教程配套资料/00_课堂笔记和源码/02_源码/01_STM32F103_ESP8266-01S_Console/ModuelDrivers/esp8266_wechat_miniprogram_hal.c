#include "uart_ring_buffer_multi.h"
#include "esp8266_handler_hal.h"
#include "esp8266_wechat_miniprogram_hal.h"
#include "string.h"
#include "gpio.h"

#define ESP8266_MINIPROGRAM_TCP 0
#define ESP8266_MINIPROGRAM_UDP 1


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define PC_UART 	&huart1
#define WIFI_UART 	&huart3


static void socket_data_handler(char *dev, char *status);

void esp8266_wechat_miniprogram_init(char *prot)
{
	uart_sendstring("wechat miniprogram mode.\r\n", PC_UART);
	#if ESP8266_MINIPROGRAM_TCP
	create_tcp_server(5000, prot);
	#elif ESP8266_MINIPROGRAM_UDP
	create_udp_server(5000, prot);
	#endif

	//while(wait_for_time_out(5000, "CONNECT", WIFI_UART) != 1)
	//	uart_sendstring("[error]Wait connect time out! Try again...\n", PC_UART);
	//uart_sendstring("[ok]Connected!\r\n", PC_UART);
	
}

// +IPD,0,43:{"room":"saloon","dev":"lamp","status":"1"}
void esp8266_wechat_miniprogram_hw_ctrl(void)
{
	char dev[32] = {'\0'};
	char status[8] = {'\0'};
	
	char Link_ID;
	while (!(get_after("+IPD,", 1, &Link_ID, WIFI_UART)));
	Link_ID -= 48;
	
	wait_for("\"dev\":\"", WIFI_UART);
	while (!(copy_upto("\"", dev, WIFI_UART)));
	dev[strlen(dev) - 1] = '\0';
	uart_sendstring(dev, PC_UART);
	
	wait_for("\"status\":\"", WIFI_UART);
	while (!(copy_upto("\"", status, WIFI_UART)));
	status[strlen(status) - 1] = '\0';
	uart_sendstring(status, PC_UART);
	
	uart_sendstring("\r\n\r\n", PC_UART);
	
	//server_handle_socket("ok", Link_ID);
	
	socket_data_handler(dev, status);
}




static void socket_data_handler(char *dev, char *status)
{
	if (strcmp(dev, "lamp1") == 0)
	{
		if (strcmp(status, "1") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);  // LED灭
	}
	else if (strcmp(dev, "lamp2") == 0)
	{
		if (strcmp(status, "1") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);  // LED灭
	}
	else if (strcmp(dev, "lamp3") == 0)
	{
		if (strcmp(status, "1") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);  // LED灭
	}
	else if(strcmp(dev, "fan") == 0)
	{
		if (strcmp(status, "1") == 0)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
	}
	else if(strcmp(dev, "home") == 0)
	{
		if (strcmp(status, "0") == 0)
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);  // LED亮
			// fan
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);  // LED灭
			//fan
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
	}
}
