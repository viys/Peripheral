/*
 * ESP8266_WEB_PAIR_HAL.h
 *
 *  Created on: 21-June-2021
 *      Author: biubiu
 */


#ifndef ESP8266_HANDLER_HAL_H_
#define ESP8266_HANDLER_HAL_H_

#include "stm32f1xx_hal.h"

uint8_t at_cmd_handler(char *cmd, char *result, uint16_t wait_time, uint16_t delay_time);  	// AT指令处理函数
char *get_dev_ip(uint16_t wait_time, uint8_t tag);											// 获取设备ip地址
uint8_t create_tcp_server(uint16_t wait_time, char *prot);    											// 创建TCP服务器
uint8_t create_udp_server(uint16_t wait_time, char *prot);    											// 创建UDP传输

void str_replace(char *target, const char *needle, const char *replacement);   // 搜索并替换字符串
int server_send (char *str, int Link_ID);
void server_handle_socket(char *str, int Link_ID);
void server_handle_select(char *html, int Link_ID);
void esp8266_restore(void);      // 恢复出厂设置

#endif /* ESP8266_HANDLER_HAL_H_ */
