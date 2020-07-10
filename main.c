#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

char USART1_RX_BUF[8]={'S','1','F','0','6','.','0','T'};
char ReturnData[5]={'0','0','0','0','0'};
u8 USART1_RX_CNT=0;
int flag=0;
char c1;
void Usart2_Init(u32 bound)
{
    //GPIO???��2��|����??
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//��1?��GPIOA����?��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//��1?��USART2����?��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //???��??��a?��a��?��o?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��GPIOA.9

    //USART1_RX      GPIOA.103?��o???��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????��o?����?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��GPIOA.10
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //???��??��a?��a��?��o?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��G
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PA10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//????��o?����?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��GPIOA.10
		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //???��??��a?��a��?��o?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��G
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//????��o?����?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��o???��GPIOA.10
		
	 //RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//?��??��??��2
  // RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//����?1?��??

    //Usart1 NVIC ????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //��|����??NVIC?D???��??����|2:2???����??��???����???��?2???��?��?|��???����??   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?����??��???����??3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //?����?��???����??3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ��a?��|�����衧o1?��1
    NVIC_Init(&NVIC_InitStructure);    //?����?Y????��|��?2?��oy3?��o???��VIC????????

   //USART 3?��o???����|����??

    USART_InitStructure.USART_BaudRate = bound;//????��22?�졧???��o
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//?��?3?��?a8??��oy?Y??��o?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//����???��a?��?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D?��?��|??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��?2?t��oy?Y����??????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //��o??��?��??����o?

    USART_Init(USART2, &USART_InitStructure); //3?��o???��????��21
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//?a??????��2?��?��o��1?D??
    USART_Cmd(USART2, ENABLE);                    //��o1?��1????��21
}
/**
* USART1?��?��?��alen???��??��2.
* buf:?��?��?��a??��o?��|��???��
* len:?��?��?��a|��??��??��2��oy(?a����?o��a?��???��2??|��??��?��o??���??,?a����???�졧�㡧|2?����a3?1y64???��??��2)
**/
void USART2_Send_Data(char *buf,u16 len)
{
    u16 t;
   
//  RS485_TX_EN=1;            //��|����???a?��?��?��a??����o?
    for(t=0;t<len;t++)        //?-??��?��?��?��a��oy?Y
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //?-??��?��?��?��a,??��|��??��?��?��a��a��o?��?
        USART_SendData(USART2,buf[t]);
    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  
//    RS485_TX_EN=0;                //��|����???a?��?��o???����o?
}

/**
* ?��?��o?????��3?��?����|��??��??��????
* ?����������??��?��o?1��???��??��?D??a21???��??��2|��??��??��????
**/
void USART2_IRQHandler(void)                    //????��21?D???��t??3��?D����
{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
  
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            ReturnData[USART1_RX_CNT] =USART_ReceiveData(USART2);
					USART1_RX_CNT++;
					if(USART1_RX_CNT==0){
						c1=ReturnData[0];
					}
     }
         //����?3?-����?1??��?����|��2����?3?D��������a?����?����SR,?����?����DR?????????����?��|??3y2???����??D??|��??��o��?a
    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) 
    {
        USART_ReceiveData(USART2);
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART2,USART_IT_RXNE); //����???�졧��a??3y?��?��o??D??
}
int main(void)
{
    Usart2_Init(9600);//????��212?�졧???��o��|����???a9600  
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		USART2_Send_Data(USART1_RX_BUF,8);
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		
    while(1)
    {       
				      
    }

}
