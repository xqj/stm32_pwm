#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

u8 USART1_RX_BUF[1];
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//使能USART1,GPIOA,C时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX      GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

    //Usart1 NVIC 配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器

   //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1
}
/**
* USART1发送len个字节.
* buf:发送区首地址
* len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
**/
void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
    GPIO_SetBits(GPIOA,GPIO_Pin_9);
//  RS485_TX_EN=1;            //设置为发送模式
    for(t=0;t<len;t++)        //循环发送数据
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        USART_SendData(USART1,buf[t]);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    GPIO_ResetBits(GPIOA,GPIO_Pin_9);
//    RS485_TX_EN=0;                //设置为接收模式
}

/**
* 接收指定长度的字符串
* 比如接收固定大小为21个字节的字符串
**/
void USART1_IRQHandler(void)                    //串口1中断服务程序
{
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART1);    //读取接收到的数据
            if(USART1_RX_CNT<1)//对于接收指定长度的字符串
            {
                USART1_RX_BUF[USART1_RX_CNT]=Res;        //记录接收到的值
                USART1_RX_CNT++;                                        //接收数据增加1
            }
     }
         //溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART1,USART_IT_RXNE); //一定要清除接收中断
}
int main(void)
{
    Usart1_Init(9600);//串口1波特率设置为9600  
    while(1)
    {       
        if(USART1_RX_CNT==1)//数据接收完成
        {
            USART1_RX_CNT=0;//指针复位
            //将接收到的数据发送出去
						USART1_RX_BUF[0]='3';
            USART1_Send_Data(USART1_RX_BUF,1);//通过串口1将接收到的固定长度字符发送出去
        }
    }

}
