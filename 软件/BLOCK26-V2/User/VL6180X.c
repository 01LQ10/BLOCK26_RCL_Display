#include "VL6180X.h"
/*============IIC Drive=============*/
I2C_HandleTypeDef I2cHandle;

void I2C_Init(void)
{
	I2cHandle.Instance = I2C;								  /* I2C */
	I2cHandle.Init.ClockSpeed = 50000;						  /* I2C通讯速度 */
	I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;				  /* I2C占空比 */
	I2cHandle.Init.OwnAddress1 = 0;							  /* I2C本机地址 */
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; /* 禁止广播呼叫 */
	I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	  /* 允许时钟延长 */
	if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
	{
		P_printf("I2c Init ERRO\n");
	}
	else
		P_printf("I2c Init OK\n");
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_I2C_CLK_ENABLE();	  /* I2C时钟使能 */
	__HAL_RCC_GPIOF_CLK_ENABLE(); /* GPIOF时钟使能 */

	/**I2C引脚配置
	PF0     ------> I2C1_SDA
	PF1     ------> I2C1_SCL
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; /* 开漏 */
	GPIO_InitStruct.Pull = GPIO_PULLUP;		/* 上拉 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_I2C; /* 复用为I2C */
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	   /* GPIO初始化 */
	/*复位I2C*/
	__HAL_RCC_I2C_FORCE_RESET();
	__HAL_RCC_I2C_RELEASE_RESET();
}

/*============IIC Drive=============*/

#define SYSRANGE__INTERMEASUREMENT_PERIOD 0x001b // P19 application notes

I2C_HandleTypeDef *i2c_dev; ///< Pointer to I2C bus interface
uint8_t VL6182_Init_state = 1;
uint8_t _i2caddr;

uint8_t VL6180X_Init(void) //(I2C_HandleTypeDef *theWire)
{
	I2C_Init();
	_i2caddr = VL6180X_DEFAULT_I2C_ADDR; // 设置iic地址
	// _i2c = theWire;

	i2c_dev = &I2cHandle;

	// check for expected model id
	if (read8(VL6180X_REG_IDENTIFICATION_MODEL_ID) != 0xB4)
	{
		P_printf("TOF_INIT ERRO[ID]\n");
		VL6182_Init_state = 0;
		return false;
	}

	// fresh out of reset?
	if (read8(VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET) & 0x01)
	{
		loadSettings();
		write8(VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET, 0x00);
	}

	return true;
}

uint8_t VL6180X_setAddress(uint8_t newAddr)
{
	write8(VL6180X_REG_SLAVE_DEVICE_ADDRESS, newAddr & 0x7F);
	_i2caddr = newAddr;
	// if (i2c_dev)
	//     delete i2c_dev;
	// i2c_dev = new I2CDevice(_i2caddr, _i2c);

	return 0;
}

uint8_t VL6180X_getAddress(void)
{
	return _i2caddr;
}

void loadSettings(void)
{
	// load settings!

	// private settings from page 24 of app note
	write8(0x0207, 0x01);
	write8(0x0208, 0x01);
	write8(0x0096, 0x00);
	write8(0x0097, 0xfd);
	write8(0x00e3, 0x00);
	write8(0x00e4, 0x04);
	write8(0x00e5, 0x02);
	write8(0x00e6, 0x01);
	write8(0x00e7, 0x03);
	write8(0x00f5, 0x02);
	write8(0x00d9, 0x05);
	write8(0x00db, 0xce);
	write8(0x00dc, 0x03);
	write8(0x00dd, 0xf8);
	write8(0x009f, 0x00);
	write8(0x00a3, 0x3c);
	write8(0x00b7, 0x00);
	write8(0x00bb, 0x3c);
	write8(0x00b2, 0x09);
	write8(0x00ca, 0x09);
	write8(0x0198, 0x01);
	write8(0x01b0, 0x17);
	write8(0x01ad, 0x00);
	write8(0x00ff, 0x05);
	write8(0x0100, 0x05);
	write8(0x0199, 0x05);
	write8(0x01a6, 0x1b);
	write8(0x01ac, 0x3e);
	write8(0x01a7, 0x1f);
	write8(0x0030, 0x00);

	// Recommended : Public registers - See data sheet for more detail
	write8(0x0011, 0x10); // Enables polling for 'New Sample ready'
						  // when measurement completes
	write8(0x010a, 0x30); // Set the averaging sample period
						  // (compromise between lower noise and
						  // increased execution time)
	write8(0x003f, 0x46); // Sets the light and dark gain (upper
						  // nibble). Dark gain should not be
						  // changed.
	write8(0x0031, 0xFF); // sets the # of range measurements after
						  // which auto calibration of system is
						  // performed
	write8(0x0041, 0x63); // Set ALS integration time to 100ms
	write8(0x002e, 0x01); // perform a single temperature calibration
						  // of the ranging sensor

	// Optional: Public registers - See data sheet for more detail
	write8(SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09); // Set default ranging inter-measurement
													 // period to 100ms
	write8(0x003e, 0x31);							 // Set default ALS inter-measurement period
													 // to 500ms
	write8(0x0014, 0x24);							 // Configures interrupt on 'New Sample
													 // Ready threshold event'
}

uint8_t VL6180X_readRange(void)
{
	uint16_t t = 0;
	// wait for device to be ready for range measurement
	if (1 == VL6182_Init_state)
	{
		while (!(read8(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01))
		{
			t++;
			if (t > 20)
			{
				T_printf("VL6180 read RANGE_STATUS erro\n");
				return 254;
			}
		}

		// Start a range measurement
		write8(VL6180X_REG_SYSRANGE_START, 0x01);

		// Poll until bit 2 is set
		t = 0;
		while (!(read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04))
		{
			t++;
			if (t > 20)
			{
				T_printf("VL6180 read INTERRUPT_STATUS erro\n");
				return 253;
			}
		}

		// read range in mm
		uint8_t range = read8(VL6180X_REG_RESULT_RANGE_VAL);

		// clear interrupt
		write8(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

		return range;
	}
	else
		return 252;
}

uint8_t VL6180X_startRange(void)
{
	// wait for device to be ready for range measurement
	while (!(read8(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01))
		;

	// Start a range measurement
	write8(VL6180X_REG_SYSRANGE_START, 0x01);

	return true;
}

uint8_t VL6180X_isRangeComplete(void)
{

	// Poll until bit 2 is set
	if ((read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04))
		return true;

	return false;
}

uint8_t VL6180X_waitRangeComplete(void)
{

	// Poll until bit 2 is set
	while (!(read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04))
		;

	return true;
}

/**************************************************************************/
/*!
	@brief  Return results of read reqyest also clears out the interrupt
	Be sure to check the return of {@link readRangeStatus} to before using
	the return value!
	@return if range started.
*/
/**************************************************************************/

uint8_t VL6180X_readRangeResult(void)
{

	// read range in mm
	uint8_t range = read8(VL6180X_REG_RESULT_RANGE_VAL);

	// clear interrupt
	write8(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

	return range;
}

void VL6180X_startRangeContinuous(uint16_t period_ms)
{
	uint8_t period_reg = 0;
	if (period_ms > 10)
	{
		if (period_ms < 2550)
			period_reg = (period_ms / 10) - 1;
		else
			period_reg = 254;
	}
	// Set  ranging inter-measurement
	write8(SYSRANGE__INTERMEASUREMENT_PERIOD, period_reg);

	// Start a continuous range measurement
	write8(VL6180X_REG_SYSRANGE_START, 0x03);
}

void VL6180X_stopRangeContinuous(void)
{
	// stop the continuous range operation, by setting the range register
	// back to 1, Page 7 of appication notes
	write8(VL6180X_REG_SYSRANGE_START, 0x01);
}

uint8_t VL6180X_readRangeStatus(void)
{
	return (read8(VL6180X_REG_RESULT_RANGE_STATUS) >> 4);
}

float VL6180X_readLux(uint8_t gain)
{
	uint8_t reg;

	reg = read8(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG);
	reg &= ~0x38;
	reg |= (0x4 << 3); // IRQ on ALS ready
	write8(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG, reg);

	// 100 ms integration period
	write8(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI, 0);
	write8(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO, 100);

	// analog gain
	if (gain > VL6180X_ALS_GAIN_40)
	{
		gain = VL6180X_ALS_GAIN_40;
	}
	write8(VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x40 | gain);

	// start ALS
	write8(VL6180X_REG_SYSALS_START, 0x1);

	// Poll until "New Sample Ready threshold event" is set
	while (4 != ((read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) >> 3) & 0x7))
		;

	// read lux!
	float lux = read16(VL6180X_REG_RESULT_ALS_VAL);

	// clear interrupt
	write8(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

	lux *= 0.32; // calibrated count/lux
	switch (gain)
	{
	case VL6180X_ALS_GAIN_1:
		break;
	case VL6180X_ALS_GAIN_1_25:
		lux /= 1.25;
		break;
	case VL6180X_ALS_GAIN_1_67:
		lux /= 1.67;
		break;
	case VL6180X_ALS_GAIN_2_5:
		lux /= 2.5;
		break;
	case VL6180X_ALS_GAIN_5:
		lux /= 5;
		break;
	case VL6180X_ALS_GAIN_10:
		lux /= 10;
		break;
	case VL6180X_ALS_GAIN_20:
		lux /= 20;
		break;
	case VL6180X_ALS_GAIN_40:
		lux /= 40;
		break;
	}
	lux *= 100;
	lux /= 100; // integration time in ms

	return lux;
}

void VL6180X_setOffset(uint8_t offset)
{
	// write the offset
	write8(VL6180X_REG_SYSRANGE_PART_TO_PART_RANGE_OFFSET, offset);
}

void VL6180X_getID(uint8_t *id_ptr)
{

	id_ptr[0] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 0);
	id_ptr[1] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 1);
	id_ptr[2] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 2);
	id_ptr[3] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 3);
	id_ptr[4] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 4);
	id_ptr[6] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 6);
	id_ptr[7] = read8(VL6180X_REG_IDENTIFICATION_MODEL_ID + 7);
}

// Read 1 byte from the VL6180X at 'address'
uint8_t read8(uint16_t address)
{
	uint8_t buffer[2];
	buffer[0] = (uint8_t)(address >> 8);
	buffer[1] = (uint8_t)(address & 0xFF);
	HAL_I2C_Master_Transmit(i2c_dev, _i2caddr, buffer, 2, 0x10);
	HAL_I2C_Master_Receive(i2c_dev, _i2caddr, buffer, 1, 0x10);
	// i2c_dev->write(buffer, 2);
	// i2c_dev->read(buffer, 1);
	return buffer[0];
}

// Read 2 byte from the VL6180X at 'address'
uint16_t read16(uint16_t address)
{
	uint8_t buffer[2];
	buffer[0] = (uint8_t)(address >> 8);
	buffer[1] = (uint8_t)(address & 0xFF);
	HAL_I2C_Master_Transmit(i2c_dev, _i2caddr, buffer, 2, 0x10);
	HAL_I2C_Master_Receive(i2c_dev, _i2caddr, buffer, 2, 0x10);
	// i2c_dev->write(buffer, 2);
	// i2c_dev->read(buffer, 2);
	return ((uint16_t)(buffer[0])) << 8 | ((uint16_t)(buffer[1]));
}

// write 1 byte
void write8(uint16_t address, uint8_t data)
{
	uint8_t buffer[3];
	buffer[0] = (uint8_t)(address >> 8);
	buffer[1] = (uint8_t)(address & 0xFF);
	buffer[2] = data;
	HAL_I2C_Master_Transmit(i2c_dev, _i2caddr, buffer, 3, 0x10);
	// i2c_dev->write(buffer, 3);
}

// write 2 bytes
void write16(uint16_t address, uint16_t data)
{
	uint8_t buffer[4];
	buffer[0] = (uint8_t)(address >> 8);
	buffer[1] = (uint8_t)(address & 0xFF);
	buffer[2] = (uint8_t)(data >> 8);
	buffer[3] = (uint8_t)(data & 0xFF);
	HAL_I2C_Master_Transmit(i2c_dev, _i2caddr, buffer, 4, 0x10);
	// i2c_dev->write(buffer, 4);
}
