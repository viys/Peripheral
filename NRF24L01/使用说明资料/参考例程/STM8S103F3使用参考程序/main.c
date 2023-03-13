

/****************************************
一帧科技工作室
单片机：STM8S103F3 
功能说明：单片机每隔3S通过串口发送6个字节出去，
          若单片机接收到6个字节之后，单片机延时
          30ms后，返回状态
QQ:420295146（小辉beyond ） 
电话：
*****************************************/

#include<iostm8s103F3.h>
#include"System_Init.h"

unsigned int Delay_count=0;   //延时函数变量
unsigned int UART_count=0;   //发送周期变量
unsigned char UART_T_flag=0;  //串口发送标志  
unsigned char UART_Data=0;
unsigned char UART_R[35];   //接收缓冲区
unsigned char R_count=0;   //接收计数 
unsigned char Receive_Deal_flag=0;  //接收处理标志位 

void Dealy(unsigned int Time)
{
  Delay_count=Time;
  while(Delay_count>0);   
}


int main(void)
{
  unsigned char i;
  asm("sim"); //disable interrupts
  CLockInit(); //初始化时钟，外部晶振，8MHZ
  Tim2Init(1000); //1ms中断一次
  GpioInit(); //初始化IO口
  UART1_Init(); //初始j化 UART
  //Watch_Dog();  //初始化看门狗   如果初始化了“看门狗”，必须得注意“喂狗”，否则，容易复位    
  R_count=0; 
  asm("rim"); //enable interrupts
  while(1)
  {
    if(UART_T_flag)   //发送周期 T=3S  ;
    {
      for(i=0;i<6;i++)   //连续发送6个字节 
      {
        UART1_TX(UART_Data);
      }
      UART_T_flag=0; 
    }
    if(Receive_Deal_flag)
    {
      /*******************
      接收命令，做动作 
      如：
      if(UART_R[0]==0x01)
      {
          ...........
      }
      ********************/
      Dealy(30);   //延时30ms后，才返回状态  
      for(i=0;i<6;i++)   //连续发送6个字节 
      {
        UART1_TX(0x68);
      }
      Receive_Deal_flag=0;
    }
    //IWDG_KR=0xAA; //"喂狗
  }
}
#pragma vector=TIM2_OVR_UIF_vector    //定时器中断
__interrupt void OneMS()  //1ms 中断一次
{
  TIM2_SR1 &=~(0x01);    // 清除TIM2溢出中断标志位
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

#pragma vector=8      //外部中断
__interrupt void Exteral_GPIO6()  
{
  
  
}
 
#pragma vector=20    //串口中断
__interrupt void UART_RXX()  
{   
   UART_R[R_count++]=UART_RX(); 
   if(R_count>6)                //接收到6个byte之后，处理 
   {
     Receive_Deal_flag=1;     
     R_count=0; 
   }
}


