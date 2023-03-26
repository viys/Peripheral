/*
 * ESP8266_WEB_PAIR_HAL.h
 *
 *  Created on: 23-June-2021
 *      Author: biubiu
 */


#ifndef ESP8266_WECHAT_MINIPROGRAM_HAL_H_
#define ESP8266_WECHAT_MINIPROGRAM_HAL_H_


void esp8266_wechat_miniprogram_init(char *prot);      // 初始化对接微信小程序(TCP&UDP Socket)
void esp8266_wechat_miniprogram_hw_ctrl(void);   // 处理与微信小程序通信数据

#endif /* ESP8266_WECHAT_MINIPROGRAM_HAL_H_ */
