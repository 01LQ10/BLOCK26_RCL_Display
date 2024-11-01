#ifndef __KEY_H
#define __KEY_H

typedef enum
{ 	KEY_D = 1,//按下
	KEY_U,//抬起
	KEY_L,//低
	KEY_H,//高
}KETSTAT_TypeDef;

void KEY_Init(void);
void KEY_SCAN(void);
u8 KEY_READ(u8 i);

#endif
