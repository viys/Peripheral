


#include<iostm8s103F3.h>
void CLockInit();
void Tim2Init(unsigned int time); //uint ms
void GpioInit();
void Watch_Dog();
void UART1_Init();
void UART1_TX(unsigned char Data);
unsigned char UART_RX();
void SPI_Init();
unsigned char SPI_TX_RX(unsigned char Data);
extern unsigned char Check_RF_Number_flag;
extern unsigned char Baud_rade_selection;