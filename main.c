#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

u8 USART1_RX_BUF[1];
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO???迆谷豕??
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//那1?邦USART1,GPIOA,C那㊣?車

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?∩車?赤?赤足那?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?那??‘GPIOA.9

    //USART1_RX      GPIOA.103?那??‘
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????那?豕?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?那??‘GPIOA.10
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?∩車?赤?赤足那?3?
    GPIO_Init(GPIOB, &GPIO_InitStructure);//3?那??‘GPIOA.9

    //Usart1 NVIC ????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //谷豕??NVIC?D??﹞?℅谷2:2???角??車??豕??㏒?2???足車|車??豕??   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?角??車??豕??3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //℅車車??豕??3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ赤“米角那1?邦
    NVIC_Init(&NVIC_InitStructure);    //?迄?Y???“米?2?那y3?那??‘VIC??∩??¯

   //USART 3?那??‘谷豕??

    USART_InitStructure.USART_BaudRate = bound;//∩??迆2“足??那
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//℅?3∟?a8??那y?Y??那?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//辰???赤㏒?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D㏒?谷??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T車2?t那y?Y芍¯????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //那?﹞⊿?㏒那?

    USART_Init(USART1, &USART_InitStructure); //3?那??‘∩??迆1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a??∩??迆?車那邦?D??
    USART_Cmd(USART1, ENABLE);                    //那1?邦∩??迆1
}
/**
* USART1﹞⊿?赤len??℅??迆.
* buf:﹞⊿?赤??那℅米??﹞
* len:﹞⊿?赤米?℅??迆那y(?a芍?o赤㊣?∩迆??米??車那??ㄓ??,?a角??“辰谷2?辰a3?1y64??℅??迆)
**/
void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
   
//  RS485_TX_EN=1;            //谷豕???a﹞⊿?赤?㏒那?
    for(t=0;t<len;t++)        //?-?﹞﹞⊿?赤那y?Y
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //?-?﹞﹞⊿?赤,?㊣米?﹞⊿?赤赤那㊣?
        USART_SendData(USART1,buf[t]);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
//    RS485_TX_EN=0;                //谷豕???a?車那??㏒那?
}

/**
* ?車那????“3∟?豕米?℅?﹞?∩?
* ㊣豕豕??車那?1足?“∩車D??a21??℅??迆米?℅?﹞?∩?
**/
void USART1_IRQHandler(void)                    //∩??迆1?D??﹞t??3足D辰
{
	
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART1);    //?芍豕??車那?米?米?那y?Y
            if(USART1_RX_CNT<1)//??車迆?車那????“3∟?豕米?℅?﹞?∩?
            {
                USART1_RX_BUF[USART1_RX_CNT]=Res;        //?????車那?米?米??米
                USART1_RX_CNT++;                                        //?車那?那y?Y???車1
            }
     }
         //辰?3?-豕?1?﹞⊿谷迆辰?3?D豕辰a?豕?芍SR,?迄?芍DR??∩??¯?辰?谷??3y2???豕??D??米??那足a
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART1,USART_IT_RXNE); //辰??“辰a??3y?車那??D??
}
int main(void)
{
    Usart1_Init(9600);//∩??迆12“足??那谷豕???a9600  
    while(1)
    {       
        if(USART1_RX_CNT==1)//那y?Y?車那?赤那3谷
        {
					 GPIO_ResetBits(GPIOB,GPIO_Pin_0);
            USART1_RX_CNT=0;//?????∩??
            //???車那?米?米?那y?Y﹞⊿?赤3?豕ㄓ
						USART1_RX_BUF[0]='6';
            USART1_Send_Data(USART1_RX_BUF,1);//赤“1y∩??迆1???車那?米?米?1足?“3∟?豕℅?﹞?﹞⊿?赤3?豕ㄓ
        }
    }

}
