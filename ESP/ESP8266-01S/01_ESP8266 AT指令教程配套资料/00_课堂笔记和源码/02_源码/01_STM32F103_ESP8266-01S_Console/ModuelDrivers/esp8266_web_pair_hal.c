/*
 * esp8266_web_pair_hal.c
 *
 *  Created on: 1-June-2021
 *      Author: biubiu
 */

#include "web_html_page.h"
#include "esp8266_handler_hal.h"
#include "uart_ring_buffer_multi.h"
#include "esp8266_web_pair_hal.h"
#include "oled_layout.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define PC_UART 	&huart1
#define WIFI_UART 	&huart3

char *home_wifi_set_page = HOME_WIFI_SET_PAGE;

static void esp_connect_to_sta(char *ssid, char *password);
//static void Web_Pair_Start(void);
//static void server_handle_select(char *html, char *str, int Link_ID);
static int server_send (char *str, int Link_ID);


static void esp_connect_to_sta(char *ssid, char *password)
{
	char cmd_buf[80];
	char *ipaddr = NULL;
	
	uart_sendstring("ESP web pair mode...", PC_UART);
	HAL_Delay(1000);
	at_cmd_handler("AT+RST\r\n", "OK\r\n", 5000, 10);
	uart_flush(WIFI_UART);
	uart_sendstring("RESETTING.", PC_UART);
	for (int i=0; i < 4; i++)
	{
		uart_sendstring(".", PC_UART);
		HAL_Delay(1000);
	}

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

	/********* AT+CIFSR **********/
	ipaddr = get_dev_ip(5000, 1);
	free(ipaddr);
	HAL_Delay(200);

	/********* AT+CIPMUX **********/
	//at_cmd_handler("AT+CIPMUX=1\r\n", "OK\r\n", 5000, 200);

	/********* AT+CIPSERVER **********/
	//at_cmd_handler("AT+CIPSERVER=1,80\r\n", "OK\r\n", 5000, 200);
	//uart_sendstring("[ok]Now Connect to the IP ADRESS.\r\n", PC_UART);
}


static void Web_Pair_Start(void)
{
	char buffer1[20] = {0};
	char buffer2[20] = {0};
	uart_flush(WIFI_UART);
	char Link_ID;
	while (!(get_after("+IPD,", 1, &Link_ID, WIFI_UART)));
	Link_ID -= 48;
	uart_sendstring("+IPD!\r\n", PC_UART);
	//server_handle("/ ", Link_ID);
	server_handle_select(home_wifi_set_page, Link_ID);

	int len = 0;
#if 1
	//uart_flush(WIFI_UART);
	uart_sendstring("wait_for ssid...\r\n", PC_UART);
	while (!(wait_for("ssid=", WIFI_UART)));
	uart_sendstring("[ok]get ssid:", PC_UART);
	uart_sendstring("\r\n", PC_UART);
	while (!(copy_upto("&",buffer1, WIFI_UART)));
	len = strlen (buffer1);
	buffer1[len-1] = '\0';
	uart_sendstring(buffer1, PC_UART);
#endif
#if 1
	uart_sendstring("wait_for password...\r\n", PC_UART);
	while (!(wait_for("password=", WIFI_UART)));
	uart_sendstring("[ok]get password:", PC_UART);
	uart_sendstring("\r\n", PC_UART);
	while (!(copy_upto("&",buffer2, WIFI_UART)));
	len = strlen (buffer2);
	buffer2[len-1] = '\0';
	uart_sendstring(buffer2, PC_UART);
#endif
	esp_connect_to_sta(buffer1, buffer2);
}



uint8_t esp_web_pair_init (char *ssid, char *password)
{
	char cmd_buf[80];
	char *ipaddr = NULL;

	uart_sendstring("AT+RST\r\n", WIFI_UART);
	uart_flush(WIFI_UART);
	uart_sendstring("[ok]RESETTING...\r\n", PC_UART);

	if (wait_for_time_out(10000, "WIFI GOT IP", WIFI_UART) == 1)
	{
		uart_sendstring("[ok]WIFI GOT IP successful!\n\n", PC_UART);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		uart_sendstring("Soft AP pairing...\r\n", PC_UART);
		/********* AT **********/
		at_cmd_handler("AT\r\n", "OK\r\n", 5000, 200);

		/********* AT+CWMODE=1 **********/
		at_cmd_handler("AT+CWMODE_CUR=2\r\n", "OK\r\n", 5000, 200);
		//at_cmd_handler("AT+CWMODE_DEF=2\r\n", "OK\r\n", 5000, 200); // 设置 Wi-Fi 模式 (STA/AP/STA+AP)，保存到 Flash

		/********* AT+CWSAP **********/
		//sprintf (cmd_buf, "AT+CWSAP_DEF=\"%s\",\"%s\",1,3,4,0\r\n", ssid, password);   // 配置 ESP8266 SoftAP 当前参数，并保存到 FLASH
		sprintf (cmd_buf, "AT+CWSAP_CUR=\"%s\",\"%s\",1,3,4,0\r\n", ssid, password);		// 配置 ESP8266 SoftAP 当前参数，不保存到 FLASH
		at_cmd_handler(cmd_buf, "OK\r\n", 5000, 200);

		/********* AT+CWDHCP_DEF **********/
		//at_cmd_handler("AT+CWDHCP_DEF=0,1\r\n", "OK\r\n", 5000, 200);// 设置 DHCP，并保存到 FALSH
		at_cmd_handler("AT+CWDHCP_CUR=0,1\r\n", "OK\r\n", 5000, 200);// 设置 DHCP，不保存到 FALSH	

		/********* AT+CIFSR **********/
		ipaddr = get_dev_ip(5000, 2);
		HAL_Delay(200);
		
		/********* AT+CIPMUX **********/
		at_cmd_handler("AT+CIPMUX=1\r\n", "OK\r\n", 5000, 200);

		/********* AT+CIPSERVER **********/
		at_cmd_handler("AT+CIPSERVER=1,80\r\n", "OK\r\n", 5000, 200);
		uart_sendstring("Now Connect to the IP ADRESS.\r\n", PC_UART);

		OLED_ClearScreen();
		OLED_DispIP(ipaddr);
		OLED_DispSoftAP(ssid, password);
		OLED_DispLogo();
		free(ipaddr);
  
		Web_Pair_Start();
	}
	return 1;
}
