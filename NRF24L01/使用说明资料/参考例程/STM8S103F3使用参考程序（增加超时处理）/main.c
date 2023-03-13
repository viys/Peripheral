

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
unsigned char Than_Time_flag=0;    //接收超时变量
unsigned char Than_Time_count=0;   //接收超时计数 

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
  
  
  if(Than_Time_flag)
  {
     Than_Time_count--;
     if(Than_Time_count>250)
     {
        Than_Time_count=100; 
        Than_Time_flag=0;    //避免下次 随便进入此段程序 
        if(R_count>=5)      //从0开始算，0 ，1 ，2, 3， 4 ，5  总共就6个字节了  
        {
           Receive_Deal_flag=1;    
        }
        R_count=0; 
     }
    
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
   /*
   if(R_count>6)                //接收到6个byte之后，处理 
   {
     Receive_Deal_flag=1;     
     R_count=0; 
   }
   */
   Than_Time_flag=1;
   Than_Time_count=30;        //至于要“超时”多少 ms  ，则根据数据的传输速度而定，这里设定成 30ms超时 
}
/**************************
我大概解释下为什么引入“超时处理”？
1、如果，串口在受到干扰时，突然间接收到一个 byte的话，此时，R_count就自增了一次，如果，没有加入“超时处理”的话，
   接收缓冲区是从   UART_R[1] 开始缓冲的，但是，你却认为单片机从 UART_R[0]开始缓冲的，这问题就来了，此时，你
   提取  “UART_R[1]” 来使用的话，程序就会出现 异常了，因为，你提取的根本就不是 “UART_R[1]”,因为”指针“偏移了 ！
2、单片机和模块通信过程中遇到干扰，突然间 ”断线 “了，这也有可能的，就有点像 打电话时 突然间 “没信号” ，
   这就相当于：  R_count 自增了 两三下后，突然间不自增了，串口若再接收到数据后，缓冲区的指针并不是从0开始，而是，
   从一个未知数开始，这也就相当于： 接收缓冲区的 ”指针“ 偏移了 ！
终上所述，加入”超时处理“可以让单片机的抗干扰能力更强，让通信更加安全，避免了因为一个错误的命令而让单片机 ”误操作了“ ！
同理，delay(30)； 延时30ms 那里也可以 改成 超时处理的，若不改成超时处理的话，程序的运行也会相对没那么稳定的，因为，你让
单片机停留在某个点 什么都不干，足足待了30ms  ，你要知道 美国航天局可以因为 误差几ms 就取消 发射 航天飞机的  ！
*****************************/







