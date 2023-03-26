/*
 * ESP8266_HAL.c
 *
 *  Created on: 1-June-2021
 *      Author: biubiu
 */
#include "web_html_page.h"
#include "esp8266_handler_hal.h"
#include "uart_ring_buffer_multi.h"
#include "esp8266_web_hw_ctrl_hal.h"
#include "gpio.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define PC_UART 	&huart1
#define WIFI_UART 	&huart3


//char *font_awesome_page = FONT_AWESOME_PAGE;
char font_awesome_page2[] =  FONT_AWESOME_PAGE2;


static void socket_data_handler(char *data);
static void ctrl_hw(char *dev, char *status);

/*****************************************************************************************************************************************/

void esp_web_hw_ctrl_init (char *ssid, char *password)
{
	char cmd_buf[80];

	uart_sendstring("ESP hardware control web server start...\r\n", PC_UART);
	HAL_Delay(1000);
	at_cmd_handler("AT+RST\r\n", "OK\r\n", 5000, 10);
	uart_flush(WIFI_UART);
	uart_sendstring("RESETTING.", PC_UART);
	
	if (wait_for_time_out(10000, "WIFI GOT IP", WIFI_UART) == 1)
	{
		uart_sendstring("[ok]WIFI GOT IP successful!\n\n", PC_UART);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		/********* AT **********/
		at_cmd_handler("AT\r\n", "OK\r\n", 5000, 200);

		/********* AT+CWMODE=1 **********/
		//at_cmd_handler("AT+CWMODE_CUR=1\r\n", "OK\r\n", 5000, 200);
		at_cmd_handler("AT+CWMODE_DEF=1\r\n", "OK\r\n", 5000, 200); // 设置 Wi-Fi 模式 (STA/AP/STA+AP)，保存到 Flash


		/********* AT+CWJAP="SSID","PASSWD" **********/
		//sprintf (cmd_buf, "AT+CWSAP=\"%s\",\"%s\"\r\n", ssid, password);
		sprintf (cmd_buf, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", ssid, password);   // 配置 ESP8266 SoftAP 当前参数，并保存到 FLASH // error
		//sprintf (cmd_buf, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, password);		// 配置 ESP8266 SoftAP 当前参数，不保存到 FLASH
		at_cmd_handler(cmd_buf, "OK\r\n", 5000, 500);
		while(wait_for_time_out(5000, "WIFI GOT IP", WIFI_UART) != 1)
			uart_sendstring("[error]Wait WIFI GOT IP Time out!\nTry again...\n", PC_UART);
		HAL_Delay(200);
	}
}


void server_handle(char *str, int Link_id)
{
	char datatosend[4096] = {0};
	
	if((strcmp(str, "dev5-off") == 0))
	{
		str_replace(font_awesome_page2, "-on", "-off");
		str_replace(font_awesome_page2, "dev5-off", "dev5-on");
		
		sprintf (datatosend, FONT_AWESOME_PAGE1);
		strcat(datatosend, font_awesome_page2);
		strcat(datatosend, FONT_AWESOME_PAGE3);	
	}
	else if((strcmp(str, "dev5-on") == 0))
	{
		str_replace(font_awesome_page2, "-off", "-on");
		str_replace(font_awesome_page2, "dev5-on", "dev5-off");
		
		sprintf (datatosend, FONT_AWESOME_PAGE1);
		strcat(datatosend, font_awesome_page2);
		strcat(datatosend, FONT_AWESOME_PAGE3);
	}
	else if ((strstr (str, "-on")))
	{
		char tmp_arry[10];
		sprintf(tmp_arry, "%s",str);
		str_replace(tmp_arry, "-on", "-off");
		str_replace(font_awesome_page2, str, tmp_arry);
		
		sprintf (datatosend, FONT_AWESOME_PAGE1);
		strcat(datatosend, font_awesome_page2);
		strcat(datatosend, FONT_AWESOME_PAGE3);
	}
	else if((strstr (str, "-off")))
	{
		char tmp_arry[10];
		sprintf(tmp_arry, "%s",str);
		//uart_sendstring(tmp_arry, PC_UART);
		str_replace(tmp_arry, "-off", "-on");
		//uart_sendstring(tmp_arry, PC_UART);
		str_replace(font_awesome_page2, str, tmp_arry);
		
		sprintf (datatosend, FONT_AWESOME_PAGE1);
		strcat(datatosend, font_awesome_page2);
		strcat(datatosend, FONT_AWESOME_PAGE3);
	}
	else
	{
		sprintf (datatosend, FONT_AWESOME_PAGE1);
		strcat(datatosend, font_awesome_page2);
		strcat(datatosend, FONT_AWESOME_PAGE3);
	}
	
	server_send(datatosend, Link_id);
}

void server_handle_socket(char *str, int Link_id)
{
	server_send(str, Link_id);
}



// +IPD,0,499:GET /r1-2-on HTTP/1.1
void esp_web_hw_ctrl_server_start (void)
{
	char buffer[16] = {0};
	char Link_id;
	while (!(get_after("+IPD,", 1, &Link_id, WIFI_UART)));
	Link_id -= 48;
	wait_for("GET /", WIFI_UART);
	while (!(copy_upto(" ", buffer, WIFI_UART)));
	int len = strlen(buffer);
	buffer[len-1] = '\0';
	
	uart_sendstring("+IPD!\r\n", PC_UART);
	uart_sendstring(buffer, PC_UART);


	if ((strcmp(buffer, "\0") != 0) && (strcmp(buffer, "favicon.ico") != 0))
	{
		socket_data_handler(buffer);
		//uart_sendstring("gfdsgfdsgfdsgfdsg\r\n", PC_UART);
	}
	
	server_handle(buffer, Link_id);
}


static void socket_data_handler(char *data)
{
	int len , count, i, j;
	char dev[8] = {0};
	char status[8] = {0}; 
	
	i = 0;
	j = 0;
	len = strlen(data);
	for(count = 0; count < len; count++)
	{
		if (data[count] == '-')
		{
			i = 0;
			j++;
			continue;
		}

		if(j == 0)
			dev[i] = data[count];
		else if(j == 1)
			status[i] = data[count];
		i++;
	}
	
	ctrl_hw(dev, status);
}



static void ctrl_hw(char *dev, char *status)
{
	if (strcmp(dev, "dev1") == 0)
	{
		if (strcmp(status, "off") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);  // LED灭
	}
	else if(strcmp(dev, "dev2") == 0)
	{
		if (strcmp(status, "off") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);  // LED灭
	}
	else if(strcmp(dev, "dev3") == 0)
	{
		if (strcmp(status, "off") == 0)
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);  // LED亮
		else
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);  // LED灭
	}
	else if(strcmp(dev, "dev4") == 0)
	{
		if (strcmp(status, "off") == 0)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET);
		}
	}
	else if(strcmp(dev, "dev5") == 0)
	{
		if (strcmp(status, "off") == 0)
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_SET);  // LED灭
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);  // LED灭
			//fan
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);  // LED亮
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8, GPIO_PIN_RESET);  // LED亮
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);  // LED亮
			// fan
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET);
			
		}
	}
	
}
