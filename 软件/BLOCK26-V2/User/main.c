// ..\tool\AirISP.exe -c air001 -p COM3 -b 115200 write_flash -e 0x08000000 .\Output\air001.hex
/**��һ����
 * ����tof������������������+�˲�
 *
 * ���������ʼ��Ĭ��ֵ [X]
 * ����������÷��� [X]
 * ��β���û�ϵͳ���Ż��㷨
 * bug1: ���״̬�������޷��ϵ磬���ܵ���VL6180��ʼ��ʧ�ܣ�VL6180��ȡ���ݿ��٣����°���ѭ�����裬���������ػ�
 * 		�������:
 * 		> ���VL6180��ʼ��״̬����ʼ��ʧ�� ��ֹ����
 * 		> ����VL6180������time_out
 */
#include "air001xx_hal.h"
#include "BLOCK_sys.h"

int main(void)
{
	BLOCK_Sys_Init();
	
	BLOCK_Sys_Start();

	while (1)
	{
		Debug_Handler();
		OS_Task_Pro_Handler();
		// TOF050_Handler();
	}
}
