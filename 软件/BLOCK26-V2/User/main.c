// ..\tool\AirISP.exe -c air001 -p COM3 -b 115200 write_flash -e 0x08000000 .\Output\air001.hex
/**下一步：
 * 完善tof驱动，可以连续测量+滤波
 *
 * 距离参数初始化默认值 [X]
 * 距离参数设置防误触 [X]
 * 结尾：用户系统，优化算法
 * bug1: 充电状态下由于无法断电，可能导致VL6180初始化失败，VL6180读取数据卡顿，导致按键循环受阻，不能正常关机
 * 		解决方案:
 * 		> 检测VL6180初始化状态，初始化失败 禁止读数
 * 		> 减少VL6180读数据time_out
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
