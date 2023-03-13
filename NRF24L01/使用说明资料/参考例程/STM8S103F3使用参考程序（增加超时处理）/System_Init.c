
#include"System_Init.h"


void CLockInit()   //����ʹ���ⲿ����8MHZ
{
  CLK_ECKR|=0x01;
  while(!CLK_ECKR&0X02);  //�ȴ��ⲿʱ���ȶ�
  CLK_PCKENR1=0X3E;  //STM8 ������ʱ��Ĭ��Ϊ�رյ�,���ԣ�Ҫ����Ӧ��ʱ��:TIME2 ,UART1 ,SPI
  CLK_SWCR|=0X02;  //���� switch
  CLK_SWR= 0XB4;  //ѡ���ⲿʱ��
  while(!CLK_CMSR&0X04);  // status register 0xb4 ����f master ѡ���� �ⲿʱ��
}
void Watch_Dog()
{
  IWDG_KR=0x55; // ���� PRE �� RLR �Ĵ���Ϊд�����͵ģ�������ȡ�������
  IWDG_PR=0X06; //��Ƶ����󣬼�����1.02s�ڣ��������û�С�ι�����Ļ�������� reset�ź�
  IWDG_RLR=0XFF;
  IWDG_KR=0xCC; //start watch dog
}
void UART1_Init()
{
   UART1_BRR2=0x01;   //8MHZ���ⲿ����,9600 �Ĳ�����   ��Ӧ�������ֽ�Ϊ�� 0X00
   UART1_BRR1=0x34;
   UART1_CR1=0x00;
   UART1_CR2=0x2c; 
}
void UART1_TX(unsigned char Data)
{   
  while(!(UART1_SR&0x80));  //��λ����1ʱ����empty��0����� not empty 
  UART1_DR=Data;
}
unsigned char SPI_TX_RX(unsigned char Data)
{
  while(!(SPI_SR&0x02));  //TX  1:����empty  0:���� not empty
  SPI_DR=Data; 
  while(!(SPI_SR&0x01));  //RX 0:����empty  1:���� not empty
  Data=SPI_DR;
  return Data;
}
unsigned char UART_RX()
{
   unsigned char DATA2 ;
   while(!(UART1_SR&0x20));  //��λ����1ʱ����empty��0����� not empty
   DATA2=UART1_DR; 
   return DATA2;
} 
void Tim2Init(unsigned int time) //uint (time *1) us
{
   TIM2_PSCR =0x03; // TIM2��ʱ��Ƶ��=fCK_PSC/2^3����8��Ƶ��8MHZ���ⲿ���񣬼�1S/1MHZ=1us������һ����������Ϊ 1us��
   TIM2_ARRH =time>>8;
   TIM2_ARRL =time&0x00ff; // TIM2_ARR��ֵ�趨Ϊ0x03e7=999������1000
   TIM2_CR1 = 0x01;            // b0 = 1,�������������
   TIM2_IER = 0x01;            // ��������ж�
}
void GpioInit()
{
  PC_DDR|=0x18; //C3 ~ C7����Ϊ���ģʽ
  PC_CR1|=0x18;  //C3�� ~ C7�� ����Ϊ�������
  PC_CR2=0; // ��Ӧ�ٶ�����Ϊ 2MHZ
  
  PB_DDR=0x00;
  PB_CR1=0x00;
  PB_CR2=0;
  
  
  PD_DDR|=0X18;  // PD3-SDA  PD4-SCL
  PD_CR1|=0X1E;  // PD1,2,3,4 ����Ϊ pull up input 
  PD_CR2=0X04;   // ��Ӧ�ٶ�����Ϊ 2MHZ ,Enable PD2���ŵ� exteral interrupt
  CPU_CCR_I0=1;
  CPU_CCR_I1=1;
  EXTI_CR1|=0X80;
    
  PD_ODR_ODR3=1;
  PD_ODR_ODR4=1;
  PC_ODR_ODR3=0;  //�˽�    �� NRF24L01�� CE ���� select bit
  PC_ODR_ODR4=1;  //�˽ž����� NRF24L01�� CSN ����
    
  
 
}
