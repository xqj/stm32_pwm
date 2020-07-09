#include "stm32f10x.h"

#define DATA_LEN 1
#define BOUND 9600

char Usart_Data_Buffer;
int count=0;

void delay_ms(u16 time)
{
    u16 i=0;
    while(time--)
    {
        i=12000;
        while(i--);
    }
}
//USART1的中断
void USART1_IRQHandler(void)
{
   
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Usart_Data_Buffer =USART_ReceiveData(USART1);//读取收到数据
    }
		//溢出-发生溢出需要先读SR，再读DR寄存器则可清除不断入中断的问题
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
    USART_ClearFlag(USART1,USART_IT_RXNE);//清除接收中断
		count++;
}
/*
蓝牙通信
pwm控制
马达速度控制
*/
int main()
{

    /*--------------------------------------GPIO-----------------------------------------------*/
    GPIO_InitTypeDef GPIO_InitStrue;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//GPIOA端口使能,对应APB2
    //TX对应蓝牙RX
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    //RX对应蓝牙TX
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);

    GPIO_InitStrue.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStrue);
    /*--------------------------------------USART-----------------------------------------------*/
    //嵌套向量中断控制器组选择
    NVIC_InitTypeDef NVIC_InitStrue;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
    //使能中断
    NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
    //抢占优先级别1
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
    //子优先级别1
    NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
    //初始化配置NVIC
    NVIC_Init(&NVIC_InitStrue);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //蓝牙
    USART_InitTypeDef USART_InitStrue;
    USART_InitStrue.USART_BaudRate=BOUND;//串口波特率
    USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//收发模式
    USART_InitStrue.USART_Parity=USART_Parity_No;//无奇偶校验位
    USART_InitStrue.USART_StopBits=USART_StopBits_1;//一个停止位
    USART_InitStrue.USART_WordLength=USART_WordLength_8b;//字长为8位数据格式

    USART_Init(USART1,&USART_InitStrue);//初始化串口1
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启接收中断
    USART_Cmd(USART1,ENABLE);//使能串口1

    while(1)
    {
			if(Usart_Data_Buffer=='1'){
				 GPIO_SetBits(GPIOB,GPIO_Pin_0);
			}
			if(Usart_Data_Buffer=='2'){
				 GPIO_ResetBits(GPIOB,GPIO_Pin_0);
			}
    }
}

