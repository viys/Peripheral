

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
unsigned char Than_Time_flag=0;    //���ճ�ʱ����
unsigned char Than_Time_count=0;   //���ճ�ʱ���� 

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
  
  
  if(Than_Time_flag)
  {
     Than_Time_count--;
     if(Than_Time_count>250)
     {
        Than_Time_count=100; 
        Than_Time_flag=0;    //�����´� ������˶γ��� 
        if(R_count>=5)      //��0��ʼ�㣬0 ��1 ��2, 3�� 4 ��5  �ܹ���6���ֽ���  
        {
           Receive_Deal_flag=1;    
        }
        R_count=0; 
     }
    
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
   /*
   if(R_count>6)                //���յ�6��byte֮�󣬴��� 
   {
     Receive_Deal_flag=1;     
     R_count=0; 
   }
   */
   Than_Time_flag=1;
   Than_Time_count=30;        //����Ҫ����ʱ������ ms  ����������ݵĴ����ٶȶ����������趨�� 30ms��ʱ 
}
/**************************
�Ҵ�Ž�����Ϊʲô���롰��ʱ������
1��������������ܵ�����ʱ��ͻȻ����յ�һ�� byte�Ļ�����ʱ��R_count��������һ�Σ������û�м��롰��ʱ�����Ļ���
   ���ջ������Ǵ�   UART_R[1] ��ʼ����ģ����ǣ���ȴ��Ϊ��Ƭ���� UART_R[0]��ʼ����ģ�����������ˣ���ʱ����
   ��ȡ  ��UART_R[1]�� ��ʹ�õĻ�������ͻ���� �쳣�ˣ���Ϊ������ȡ�ĸ����Ͳ��� ��UART_R[1]��,��Ϊ��ָ�롰ƫ���� ��
2����Ƭ����ģ��ͨ�Ź������������ţ�ͻȻ�� ������ ���ˣ���Ҳ�п��ܵģ����е��� ��绰ʱ ͻȻ�� ��û�źš� ��
   ����൱�ڣ�  R_count ������ �����º�ͻȻ�䲻�����ˣ��������ٽ��յ����ݺ󣬻�������ָ�벢���Ǵ�0��ʼ�����ǣ�
   ��һ��δ֪����ʼ����Ҳ���൱�ڣ� ���ջ������� ��ָ�롰 ƫ���� ��
�������������롱��ʱ���������õ�Ƭ���Ŀ�����������ǿ����ͨ�Ÿ��Ӱ�ȫ����������Ϊһ�������������õ�Ƭ�� ��������ˡ� ��
ͬ��delay(30)�� ��ʱ30ms ����Ҳ���� �ĳ� ��ʱ����ģ������ĳɳ�ʱ����Ļ������������Ҳ�����û��ô�ȶ��ģ���Ϊ������
��Ƭ��ͣ����ĳ���� ʲô�����ɣ��������30ms  ����Ҫ֪�� ��������ֿ�����Ϊ ��ms ��ȡ�� ���� ����ɻ���  ��
*****************************/







