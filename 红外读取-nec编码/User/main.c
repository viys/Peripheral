//////////////////////////////////////////////////////////////////////////////////	 
//ƽ̨������stm32f10x
//���ܣ�IIC������ȡeeprom����  usart�ж϶�ȡ�ش�  sys��ʱ��    
//���ߣ�����С���
//�޸�����:2019/8/25
//˵��������ƽ̨eepromAT24c256 
//      �������к�����ϸ�ӿڴ�����ṩ����ѧϰʹ�ã�������ҵ��;����Ը�									  
//////////////////////////////////////////////////////////////////////////////////

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_usart.h"
#include "nec_read.h" 


extern uint8_t readstr[100],usart_i; //�������ݱ���� 
 
/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */

extern uint8_t gIrCode[5]; // gIrCode��0-3������ԭʼ���ݣ�4�����ž���У��ȷ������ļ�ֵ



int main(void)
{	
	USART_Config();       //���ڳ�ʼ��
  led_gpio_config();    //led���ų�ʼ��
	exit_key_config();    //�ⲿ�жϳ�ʼ��
  printf( "����һ��������Գ���\n");
	while (1)
	{

//   if(readstr[usart_i-1] == 0x0a)
//	 {
//		 LED_G_TOGGLE;
// 		Usart_SendStr(DEBUG_USARTx ,readstr); //��������ش�
//		 printf("%x %x %x %x %x \n",gIrCode[0],gIrCode[1],gIrCode[2],gIrCode[3],gIrCode[4]);
//		usart_i=0;
//	 }
	}
}
/*********************************************END OF FILE**********************/
