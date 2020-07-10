#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

char USART1_RX_BUF[8]={'S','1','F','0','6','.','0','T'};
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO???������??
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?�䨮?��?������?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��??��GPIOA.9

    //USART1_RX      GPIOA.103?��??��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????��?��?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��??��GPIOA.10
	
	 RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
   RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ

    //Usart1 NVIC ????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //����??NVIC?D??��?����2:2???��??��??��??��?2???����|��??��??   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?��??��??��??3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //������??��??3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ�����̨���1?��
    NVIC_Init(&NVIC_InitStructure);    //?��?Y???����?2?��y3?��??��VIC??��??��

   //USART 3?��??������??

    USART_InitStructure.USART_BaudRate = bound;//��??��2����??��
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��?3��?a8??��y?Y??��?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//��???����?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D��?��??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��2?t��y?Y����????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //��?����?�꨺?

    USART_Init(USART2, &USART_InitStructure); //3?��??����??��1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//?a??��??��?������?D??
    USART_Cmd(USART2, ENABLE);                    //��1?����??��1
}
/**
* USART1����?��len??��??��.
* buf:����?��??������??��
* len:����?����?��??����y(?a��?o����?�䨲??��??����??��??,?a��??������2?��a3?1y64??��??��)
**/
void USART2_Send_Data(char *buf,u16 len)
{
    u16 t;
   
//  RS485_TX_EN=1;            //����???a����?��?�꨺?
    for(t=0;t<len;t++)        //?-?������?����y?Y
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //?-?������?��,?����?����?��������?
        USART_SendData(USART2,buf[t]);
    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  
//    RS485_TX_EN=0;                //����???a?����??�꨺?
}

/**
* ?����????��3��?����?��?��?��?
* ������??����?1��?���䨮D??a21??��??����?��?��?��?
**/
void USART2_IRQHandler(void)                    //��??��1?D??��t??3��D��
{
	
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART2); 
     }
         //��?3?-��?1?���騦����?3?D����a?��?��SR,?��?��DR??��??��?��?��??3y2???��??D??��??����a
    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) 
    {
        USART_ReceiveData(USART2);
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART2,USART_IT_RXNE); //��??����a??3y?����??D??
}
int main(void)
{
    Usart2_Init(9600);//��??��12����??������???a9600  
	 
    while(1)
    {       
       
    }

}
