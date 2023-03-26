//////////////////////////////////////////////////////////////////////////////////	 
//平台：基于stm32f10x
//功能：IIC驱动读取eeprom代码  usart中断读取回传  sys定时器    
//作者：亚洲小甜瓜
//修改日期:2019/8/25
//说明：测试平台eepromAT24c256 
//      给出所有函数详细接口代码仅提供交流学习使用，其它商业用途后果自负									  
//////////////////////////////////////////////////////////////////////////////////

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "nec_read.h" 


extern uint8_t readstr[100],usart_i; //串口数据保存地 
 
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */

extern uint8_t gIrCode[5]; // gIrCode的0-3用来放原始数据，4用来放经过校验确认无误的键值



int main(void)
{	
	USART_Config();       //串口初始化
  led_gpio_config();    //led引脚初始化
	exit_key_config();    //外部中断初始化
  printf( "这是一个红外测试程序\n");
	while (1)
	{

//   if(readstr[usart_i-1] == 0x0a)
//	 {
//		 LED_G_TOGGLE;
// 		Usart_SendStr(DEBUG_USARTx ,readstr); //串口输入回传
//		 printf("%x %x %x %x %x \n",gIrCode[0],gIrCode[1],gIrCode[2],gIrCode[3],gIrCode[4]);
//		usart_i=0;
//	 }
	}
}
/*********************************************END OF FILE**********************/
