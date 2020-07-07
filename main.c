#include "stm32f10x.h"


char  Usart2_Data_Buffer;

//USART2的中断
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
    {
       Usart2_Data_Buffer = USART_ReceiveData(USART2);      
    }
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOA端口使能,对应APB2
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
    //马达控制
    //左1
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_5;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_6;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);   
    /*--------------------------------------USART-----------------------------------------------*/
    //蓝牙
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    //pwm控制
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    USART_InitTypeDef USART_InitStrue;
    NVIC_InitTypeDef NVIC_InitStrue;
    USART_InitStrue.USART_BaudRate=9600;
    USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
    USART_InitStrue.USART_Parity=USART_Parity_No;
    USART_InitStrue.USART_StopBits=USART_StopBits_1;
    USART_InitStrue.USART_WordLength=USART_WordLength_8b;
    USART_Init(USART2,&USART_InitStrue);
    USART_Cmd(USART2,ENABLE);//使能串口2
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//开启接收中断
    //嵌套向量中断控制器组选择
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
    //--------------------------------------------//
    USART_Init(USART1,&USART_InitStrue);
    USART_Cmd(USART1,ENABLE);//使能串口1
    //马达初始化，停止
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    GPIO_ResetBits(GPIOA, GPIO_Pin_6);  
    while(1)
    {
       if(Usart2_Data_Buffer==1){
				  GPIO_SetBits(GPIOA, GPIO_Pin_5);
					GPIO_ResetBits(GPIOA, GPIO_Pin_6);  
			 }
			 if(Usart2_Data_Buffer==2){
				  GPIO_ResetBits(GPIOA, GPIO_Pin_5);
					GPIO_ResetBits(GPIOA, GPIO_Pin_6);  
			 }
    }
}

