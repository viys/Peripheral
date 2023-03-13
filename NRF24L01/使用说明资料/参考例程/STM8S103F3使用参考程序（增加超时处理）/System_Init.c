
#include"System_Init.h"


void CLockInit()   //打算使用外部晶振8MHZ
{
  CLK_ECKR|=0x01;
  while(!CLK_ECKR&0X02);  //等待外部时钟稳定
  CLK_PCKENR1=0X3E;  //STM8 的外设时钟默认为关闭的,所以，要打开相应的时钟:TIME2 ,UART1 ,SPI
  CLK_SWCR|=0X02;  //允许 switch
  CLK_SWR= 0XB4;  //选择外部时钟
  while(!CLK_CMSR&0X04);  // status register 0xb4 代表：f master 选择了 外部时钟
}
void Watch_Dog()
{
  IWDG_KR=0x55; // 由于 PRE 与 RLR 寄存器为写保护型的，必须得先“解锁”
  IWDG_PR=0X06; //分频计算后，即：在1.02s内，如果，还没有“喂狗”的话，会产生 reset信号
  IWDG_RLR=0XFF;
  IWDG_KR=0xCC; //start watch dog
}
void UART1_Init()
{
   UART1_BRR2=0x01;   //8MHZ的外部晶振,9600 的波特率   对应的配置字节为： 0X00
   UART1_BRR1=0x34;
   UART1_CR1=0x00;
   UART1_CR2=0x2c; 
}
void UART1_TX(unsigned char Data)
{   
  while(!(UART1_SR&0x80));  //此位等于1时代表empty，0则代表 not empty 
  UART1_DR=Data;
}
unsigned char SPI_TX_RX(unsigned char Data)
{
  while(!(SPI_SR&0x02));  //TX  1:代表empty  0:代表 not empty
  SPI_DR=Data; 
  while(!(SPI_SR&0x01));  //RX 0:代表empty  1:代表 not empty
  Data=SPI_DR;
  return Data;
}
unsigned char UART_RX()
{
   unsigned char DATA2 ;
   while(!(UART1_SR&0x20));  //此位等于1时代表empty，0则代表 not empty
   DATA2=UART1_DR; 
   return DATA2;
} 
void Tim2Init(unsigned int time) //uint (time *1) us
{
   TIM2_PSCR =0x03; // TIM2的时钟频率=fCK_PSC/2^3，即8分频，8MHZ的外部晶振，即1S/1MHZ=1us，即计一个数的周期为 1us，
   TIM2_ARRH =time>>8;
   TIM2_ARRL =time&0x00ff; // TIM2_ARR初值设定为0x03e7=999，计数1000
   TIM2_CR1 = 0x01;            // b0 = 1,允许计数器工作
   TIM2_IER = 0x01;            // 允许更新中断
}
void GpioInit()
{
  PC_DDR|=0x18; //C3 ~ C7设置为输出模式
  PC_CR1|=0x18;  //C3口 ~ C7口 设置为推免输出
  PC_CR2=0; // 反应速度设置为 2MHZ
  
  PB_DDR=0x00;
  PB_CR1=0x00;
  PB_CR2=0;
  
  
  PD_DDR|=0X18;  // PD3-SDA  PD4-SCL
  PD_CR1|=0X1E;  // PD1,2,3,4 设置为 pull up input 
  PD_CR2=0X04;   // 反应速度设置为 2MHZ ,Enable PD2引脚的 exteral interrupt
  CPU_CCR_I0=1;
  CPU_CCR_I1=1;
  EXTI_CR1|=0X80;
    
  PD_ODR_ODR3=1;
  PD_ODR_ODR4=1;
  PC_ODR_ODR3=0;  //此脚    与 NRF24L01的 CE 相连 select bit
  PC_ODR_ODR4=1;  //此脚就是与 NRF24L01的 CSN 相连
    
  
 
}
