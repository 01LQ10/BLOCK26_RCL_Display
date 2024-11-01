#ifndef __DEBUG_LOG_H
#define __DEBUG_LOG_H

/*ͷ�ļ�*/
#include "air001xx_hal.h"

/** Debug print ���� 
 * Print	������������ڿ��������ʼ����Ϣ
 * User		�û����������ϵͳ�����û����ƴ�log���
 * Test		�������������׶ο����Ƿ���������ڲ�����Ϣ
 */
// ����T_print�꣬����debug_enable��ֵ�����Ƿ����
#define P_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

extern uint8_t U_PRINT_ENABLE; // �û���ӡʹ��
#define U_printf(fmt, ...) if(U_PRINT_ENABLE){printf(fmt, ##__VA_ARGS__);}

#define T_Debug_Enable 1
#if T_Debug_Enable
#define T_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define T_printf(fmt, ...)
#endif

#endif

