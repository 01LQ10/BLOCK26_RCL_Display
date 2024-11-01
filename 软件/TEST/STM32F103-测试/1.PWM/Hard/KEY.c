#include "stm32f10x.h"
#include "KEY.h"

#define KEY_NUM 2  //按键数量
#define GPIO_KEY GPIOA
#define GPIO_Pin_KEY GPIO_Pin_8|GPIO_Pin_9

uint16_t KEY[KEY_NUM]={GPIO_Pin_8, GPIO_Pin_9};//引脚映射
u8 LAST[KEY_NUM],NOW[KEY_NUM];//缓存状态


void KEY_Init(void)//按键初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIO时钟
	
	GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO初始化结构体
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_KEY;
	GPIO_Init(GPIO_KEY, &GPIO_InitStruct);
	
	for(u8 i=0;i<KEY_NUM;i++)//初始化数组
	{
		LAST[i] = KEY_H;
		NOW[i] = KEY_H;
	}
}


void KEY_SCAN(void)//按键扫描
{
	for(u8 i=0;i<KEY_NUM;i++)
	{
		LAST[i] = NOW[i];
		if(GPIO_ReadInputDataBit(GPIO_KEY, KEY[i]) == Bit_SET)
			 NOW[i] = KEY_H;
		else
			NOW[i] = KEY_L;
	}
}
u8 KEY_READ(u8 i)//按键读取
{
	if(NOW[i] == KEY_H)
	{
		if(LAST[i] == KEY_H)
			return (u8)KEY_H;
		else
			return (u8)KEY_U;
	}
	
	else if(NOW[i] == KEY_L)
	{
		if(LAST[i] == KEY_L)
			return (u8)KEY_L;
		else
			return (u8)KEY_D;
	}
	
	else
		return 0;
}	

