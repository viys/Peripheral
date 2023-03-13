

/****************************************
һ֡�Ƽ�������
��Ƭ����STM8S103F3 
����˵������Ƭ��ÿ��3Sͨ�����ڷ���6���ֽڳ�ȥ��
          ����Ƭ�����յ�6���ֽ�֮�󣬵�Ƭ����ʱ
          30ms�󣬷���״̬
QQ:420295146��С��beyond �� 
�绰��
*****************************************/

#include<iostm8s103F3.h>
#include"System_Init.h"

unsigned int Delay_count=0;   //��ʱ��������
unsigned int UART_count=0;   //�������ڱ���
unsigned char UART_T_flag=0;  //���ڷ��ͱ�־  
unsigned char UART_Data=0;
unsigned char UART_R[35];   //���ջ�����
unsigned char R_count=0;   //���ռ��� 
unsigned char Receive_Deal_flag=0;  //���մ����־λ 

void Dealy(unsigned int Time)
{
  Delay_count=Time;
  while(Delay_count>0);   
}


int main(void)
{
  unsigned char i;
  asm("sim"); //disable interrupts
  CLockInit(); //��ʼ��ʱ�ӣ��ⲿ����8MHZ
  Tim2Init(1000); //1ms�ж�һ��
  GpioInit(); //��ʼ��IO��
  UART1_Init(); //��ʼj�� UART
  //Watch_Dog();  //��ʼ�����Ź�   �����ʼ���ˡ����Ź����������ע�⡰ι�������������׸�λ    
  R_count=0; 
  asm("rim"); //enable interrupts
  while(1)
  {
    if(UART_T_flag)   //�������� T=3S  ;
    {
      for(i=0;i<6;i++)   //��������6���ֽ� 
      {
        UART1_TX(UART_Data);
      }
      UART_T_flag=0; 
    }
    if(Receive_Deal_flag)
    {
      /*******************
      ������������� 
      �磺
      if(UART_R[0]==0x01)
      {
          ...........
      }
      ********************/
      Dealy(30);   //��ʱ30ms�󣬲ŷ���״̬  
      for(i=0;i<6;i++)   //��������6���ֽ� 
      {
        UART1_TX(0x68);
      }
      Receive_Deal_flag=0;
    }
    //IWDG_KR=0xAA; //"ι��
  }
}
#pragma vector=TIM2_OVR_UIF_vector    //��ʱ���ж�
__interrupt void OneMS()  //1ms �ж�һ��
{
  TIM2_SR1 &=~(0x01);    // ���TIM2����жϱ�־λ
  Delay_count--;
  if(Delay_count>65534)
    Delay_count=0;
  
  UART_count++;
  if(UART_count>=3000)
  { 
    UART_Data++;
    UART_T_flag=1;
    UART_count=0; 
  }
  
}

#pragma vector=8      //�ⲿ�ж�
__interrupt void Exteral_GPIO6()  
{
  
  
}
 
#pragma vector=20    //�����ж�
__interrupt void UART_RXX()  
{   
   UART_R[R_count++]=UART_RX(); 
   if(R_count>6)                //���յ�6��byte֮�󣬴��� 
   {
     Receive_Deal_flag=1;     
     R_count=0; 
   }
}


