#include "uart_ring_buffer_multi.h"
#include "esp8266_handler_hal.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#define PC_UART 	&huart1
#define WIFI_UART 	&huart3

/*Function to replace a string with another string*/


void str_replace(char *target, const char *needle, const char *replacement)
{
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}


int server_send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	uart_sendstring(data, WIFI_UART);
	while (!(wait_for(">", WIFI_UART)));
	uart_sendstring (str, WIFI_UART);
	while (!(wait_for("SEND OK", WIFI_UART)));
	sprintf (data, "AT+CIPCLOSE=5\r\n");
	uart_sendstring(data, WIFI_UART);
	while (!(wait_for("OK\r\n", WIFI_UART)));
	return 1;
}

void server_handle_select(char *html, int Link_ID)
{
	server_send(html, Link_ID);
}

uint8_t at_cmd_handler(char *cmd, char *result, uint16_t wait_time, uint16_t delay_time)
{
	uart_flush(WIFI_UART);
	uart_sendstring(cmd, WIFI_UART);
	if(wait_for_time_out(5000, result, WIFI_UART) == 1)
	{
		uart_sendstring("[ok]", PC_UART);
		uart_sendstring(cmd, PC_UART);
		uart_sendstring("\r\n", PC_UART);
		
		return 1;
	}
	else
	{
		uart_sendstring("[error]", PC_UART);
		uart_sendstring(cmd, PC_UART);
		uart_sendstring("\r\n", PC_UART);
	}
	
	HAL_Delay(delay_time);
	return 0;
	
}


// 获取设备ip地址  tag=1:SoftAP	IP ; tag=2:tation	IP
//+CIFSR:STAIP,"192.168.0.171"
//+CIFSR:STAMAC,"48:3f:da:20:34:37"

char *get_dev_ip(uint16_t wait_time, uint8_t tag)
{
	char * buffer =  (char *) malloc(20);
	
	/********* AT+CIFSR **********/
	uart_flush(WIFI_UART);
	uart_sendstring("AT+CIFSR\r\n", WIFI_UART);			// 查看 ESP8266 的 IP 地址
	
	if (tag == 1)	wait_for_time_out(wait_time, "+CIFSR:STAIP,\"", WIFI_UART);
	else			wait_for_time_out(wait_time, "+CIFSR:APIP,\"", WIFI_UART);
	
	while (!(copy_upto("\"",buffer, WIFI_UART)));
	wait_for_time_out(wait_time, "OK\r\n", WIFI_UART);
	
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	uart_sendstring("IP ADDR: ", PC_UART);
	uart_sendstring(buffer, PC_UART);
	uart_sendstring("\r\n", PC_UART);
	
	return buffer;  // TODO
}


// 创建TCP服务器
uint8_t create_tcp_server(uint16_t wait_time, char *prot)
{
	char cmd[32];
	char *ipaddr = NULL;
	sprintf (cmd, "AT+CIPSERVER=1,%s\r\n", prot);
	
	ipaddr = get_dev_ip(wait_time, 1);
	free(ipaddr);
	HAL_Delay(200);
	
	/********* AT+CIPMUX **********/
	uart_flush(WIFI_UART);
	uart_sendstring("AT+CIPMUX=1\r\n", WIFI_UART);
	wait_for_time_out(wait_time, "OK\r\n", WIFI_UART);
	uart_sendstring("[ok]CIPMUX=1\r\n", PC_UART);
	HAL_Delay(200);

	/********* AT+CIPSERVER **********/
	uart_flush(WIFI_UART);
	//uart_sendstring("AT+CIPSERVER=1,80\r\n", WIFI_UART);
	uart_sendstring(cmd, WIFI_UART);
	wait_for_time_out(wait_time, "OK\r\n", WIFI_UART);
	//uart_sendstring("[ok]CIPSERVER=1,9999\r\n", PC_UART);
	uart_sendstring("[ok]", PC_UART);
	uart_sendstring(cmd, PC_UART);
	
	uart_sendstring("Now Connect to the IP ADRESS\r\n", PC_UART);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	
	return 1;
}



// 创建 UDP 传输
uint8_t create_udp_server(uint16_t wait_time, char *prot)
{
	char cmd[64];
	char *ipaddr = NULL;
	
	sprintf (cmd, "AT+CIPSTART=\"UDP\",\"127.0.0.1\",%s,%s,0\r\n", prot, prot);
	
	/********* AT+CIFSR **********/
	ipaddr = get_dev_ip(wait_time, 1);
	free(ipaddr);
	HAL_Delay(200);
	
	/********* AT+CWMODE=1 **********/
	//at_cmd_handler("AT+CWMODE=1\r\n", "OK\r\n", 5000, 100);
	
	/********* AT+CIPMUX=0 建立单连接**********/
	at_cmd_handler("AT+CIPMUX=0\r\n", "OK\r\n", 5000, 100);
	uart_sendstring("[ok]AT+CIPMUX=0\r\n", PC_UART);
	
	/********* AT+CIPSTART **********/ // AT+CIPSTART="UDP","127.0.0.1",8080,8080,0
	//at_cmd_handler("AT+CIPCLOSE\r\n", "OK\r\n", 5000, 100);  // 断开连接
	//at_cmd_handler("AT+CIPSTART=\"UDP\",\"127.0.0.1\",9999,9999,0\r\n", "OK\r\n", 5000, 100);  // 建立 UDP 传输
	at_cmd_handler(cmd, "OK\r\n", 5000, 100);  // 建立 UDP 传输
	uart_sendstring("[ok]AT+CIPSTART=\"UDP\",\"127.0.0.1\",9999,9999,0\r\n", PC_UART);
	
	uart_sendstring("Now Connect to the IP ADRESS\r\n", PC_UART);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	
	return 1;
}


void esp8266_restore(void)
{
	uart_sendstring("!!!ATTENTION DEVICE RESTORE!!!\r\n", PC_UART);
	at_cmd_handler("AT+RESTORE\r\n", "OK\r\n", 1000, 0);
}



