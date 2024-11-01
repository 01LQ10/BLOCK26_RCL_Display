#include "TOF_IIC.h"

I2C_HandleTypeDef hi2c1;
// I2C1的初始化函数
void I2C1_Init(void)
{
  hi2c1.Instance = I2C1;         // 使用I2C1
  hi2c1.Init.ClockSpeed = 10000; // 通信速率100K
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1); // 初始化I2C1
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitType; // IO口配置的结构体

  if (hi2c->Instance == I2C1)
  {
    __HAL_RCC_I2C1_CLK_ENABLE();  // 打开I2C1的时钟
    __HAL_RCC_GPIOB_CLK_ENABLE(); // 打开B口时钟

    GPIO_InitType.Pin = GPIO_PIN_6 | GPIO_PIN_7; // 配置PIN6 7
    GPIO_InitType.Mode = GPIO_MODE_AF_OD;        // 复用开漏功能
    GPIO_InitType.Speed = GPIO_SPEED_FREQ_LOW;   // 速度低速
    HAL_GPIO_Init(GPIOB, &GPIO_InitType);        // 配置PB0 1
  }
}

const uint16_t ScalerValues[] = {0, 253, 127, 84};
uint8_t ptp_offset;
uint8_t scaling;
uint16_t io_timeout;

void VL6180X_WR_CMD(uint16_t cmd, uint8_t data)
{
  uint8_t data_write[3];
  data_write[0] = (cmd >> 8) & 0xff;
  data_write[1] = cmd & 0xff;
  data_write[2] = data & 0xff;
  HAL_I2C_Master_Transmit(&hi2c1, addr_write, data_write, 3, 0x100);
}

void VL6180X_WR_CMD2(uint16_t cmd, uint16_t data)
{
  uint8_t data_write[4];
  data_write[0] = (cmd >> 8) & 0xff;
  data_write[1] = cmd & 0xff;
  data_write[2] = (data >> 8) & 0xff;
  data_write[3] = data & 0xff;
  HAL_I2C_Master_Transmit(&hi2c1, addr_write, data_write, 4, 0x100);
}

uint8_t VL6180X_ReadByte(uint16_t reg)
{
  uint8_t data_write[2];
  uint8_t receive_data = 0;
  data_write[0] = (reg >> 8) & 0xff;
  data_write[1] = reg & 0xff;
  HAL_I2C_Master_Transmit(&hi2c1, addr_write, data_write, 2, 0x100);
  HAL_I2C_Master_Receive(&hi2c1, addr_read, &receive_data, 1, 0x100);
  return receive_data;
}

uint8_t VL6180X_init()
{
  ptp_offset = 0;
  scaling = 0;
  io_timeout = 2;

  ptp_offset = VL6180X_ReadByte(SYSRANGE__PART_TO_PART_RANGE_OFFSET);
  uint8_t reset = VL6180X_ReadByte(0x016); // check wether reset over
  if (reset == 1)
  {
    VL6180X_WR_CMD(0X0207, 0X01);
    VL6180X_WR_CMD(0X0208, 0X01);
    VL6180X_WR_CMD(0X0096, 0X00);
    VL6180X_WR_CMD(0X0097, 0XFD);
    VL6180X_WR_CMD(0X00E3, 0X00);
    VL6180X_WR_CMD(0X00E4, 0X04);
    VL6180X_WR_CMD(0X00E5, 0X02);
    VL6180X_WR_CMD(0X00E6, 0X01);
    VL6180X_WR_CMD(0X00E7, 0X03);
    VL6180X_WR_CMD(0X00F5, 0X02);
    VL6180X_WR_CMD(0X00D9, 0X05);
    VL6180X_WR_CMD(0X00DB, 0XCE);
    VL6180X_WR_CMD(0X02DC, 0X03);
    VL6180X_WR_CMD(0X00DD, 0XF8);
    VL6180X_WR_CMD(0X009F, 0X00);
    VL6180X_WR_CMD(0X00A3, 0X3C);
    VL6180X_WR_CMD(0X00B7, 0X00);
    VL6180X_WR_CMD(0X00BB, 0X3C);
    VL6180X_WR_CMD(0X00B2, 0X09);
    VL6180X_WR_CMD(0X00CA, 0X09);
    VL6180X_WR_CMD(0X0198, 0X01);
    VL6180X_WR_CMD(0X01B0, 0X17);
    VL6180X_WR_CMD(0X01AD, 0X00);
    VL6180X_WR_CMD(0X00FF, 0X05);
    VL6180X_WR_CMD(0X0100, 0X05);
    VL6180X_WR_CMD(0X0199, 0X05);
    VL6180X_WR_CMD(0X01A6, 0X1B);
    VL6180X_WR_CMD(0X01AC, 0X3E);
    VL6180X_WR_CMD(0X01A7, 0X1F);
    VL6180X_WR_CMD(0X0030, 0X00);

    VL6180X_WR_CMD(0X0011, 0X10);
    VL6180X_WR_CMD(0X010A, 0X30);
    VL6180X_WR_CMD(0X003F, 0X46);
    VL6180X_WR_CMD(0X0031, 0XFF);
    VL6180X_WR_CMD(0X0040, 0X63);
    VL6180X_WR_CMD(0X002E, 0X01);
    VL6180X_WR_CMD(0X001B, 0X09);
    VL6180X_WR_CMD(0X003E, 0X31);
    VL6180X_WR_CMD(0X0014, 0X24);

    VL6180X_WR_CMD(0x016, 0x00);
    return 1;
  }
  return 0;
}

void VL6180X_SetScaling(uint8_t new_scaling)
{
  uint8_t const DefaultCrosstalkValidHeight = 20; // default value of SYSRANGE__CROSSTALK_VALID_HEIGHT

  // do nothing if scaling value is invalid
  if (new_scaling < 1 || new_scaling > 3)
  {
    return;
  }

  scaling = new_scaling;
  VL6180X_WR_CMD2(RANGE_SCALER, ScalerValues[scaling]);

  // apply scaling on part-to-part offset
  VL6180X_WR_CMD(SYSRANGE__PART_TO_PART_RANGE_OFFSET, ptp_offset / scaling);

  // apply scaling on CrossTalkValidHeight
  VL6180X_WR_CMD(SYSRANGE__CROSSTALK_VALID_HEIGHT, DefaultCrosstalkValidHeight / scaling);

  // This function does not apply scaling to RANGE_IGNORE_VALID_HEIGHT.

  // enable early convergence estimate only at 1x scaling
  uint8_t rce = VL6180X_ReadByte(SYSRANGE__RANGE_CHECK_ENABLES);
  VL6180X_WR_CMD(SYSRANGE__RANGE_CHECK_ENABLES, (rce & 0xFE) | (scaling == 1));
}

void VL6180X_ConfigureDefault()
{
  VL6180X_WR_CMD(READOUT__AVERAGING_SAMPLE_PERIOD, 0x30);
  VL6180X_WR_CMD(SYSALS__ANALOGUE_GAIN, 0x46);
  VL6180X_WR_CMD(SYSRANGE__VHV_REPEAT_RATE, 0xFF);
  VL6180X_WR_CMD2(SYSALS__INTEGRATION_PERIOD, 0x0063);
  VL6180X_WR_CMD(SYSRANGE__VHV_RECALIBRATE, 0x01);
  VL6180X_WR_CMD(SYSRANGE__INTERMEASUREMENT_PERIOD, 0x09);
  VL6180X_WR_CMD(SYSALS__INTERMEASUREMENT_PERIOD, 0x31);
  VL6180X_WR_CMD(SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24);
  VL6180X_WR_CMD(SYSRANGE__MAX_CONVERGENCE_TIME, 0x31);
  VL6180X_WR_CMD(INTERLEAVED_MODE__ENABLE, 0);
  VL6180X_SetScaling(1);
}

void VL6180X_SetTimeout(uint16_t timeout)
{
  io_timeout = timeout;
}

uint8_t VL6180X_Start_Range()
{
  VL6180X_WR_CMD(0x018, 0x01);
  return 0;
}

uint16_t timeoutcnt = 0;

/*poll for new sample ready */
uint8_t VL6180X_Poll_Range()
{
  uint8_t status;
  uint8_t range_status;
  status = VL6180X_ReadByte(0x04f);
  range_status = status & 0x07;
  while (range_status != 0x04)
  {
    timeoutcnt++;
    if (timeoutcnt > io_timeout)
    {
      break;
    }
    status = VL6180X_ReadByte(0x04f);
    range_status = status & 0x07;
    HAL_Delay(1);
  }
  return 0;
}

/*read range result (mm)*/
uint8_t VL6180_Read_Range()
{
  int range;
  range = VL6180X_ReadByte(0x062);
  return range;
}

/*clear interrupt*/
void VL6180X_Clear_Interrupt()
{
  VL6180X_WR_CMD(0x015, 0x07);
}

uint16_t VL6180X_ReadRangeSingleMillimeters()
{
  /*Start Single measure mode*/
  VL6180X_Start_Range();
  /* Wait for measurement ready. */
  VL6180X_Poll_Range();
  HAL_Delay(100);
  return (uint16_t)scaling * VL6180_Read_Range();
}

uint8_t RangePollingRead(void)
{
  uint8_t status;
  uint8_t distance;

  /* Wait for device ready. */
  do
  {
    status = VL6180X_ReadByte(RESULT__RANGE_STATUS);
  } while ((status & (1 << 0)) == 0);

  VL6180X_WR_CMD(SYSRANGE__START, 0X01);
  VL6180X_ReadByte(SYSRANGE__START);

  status = VL6180X_ReadByte(RESULT__INTERRUPT_STATUS_GPIO);
  status = status & 0x07;

  /* Wait for measurement ready. */
  while (status != 0x04)
  {
    status = VL6180X_ReadByte(RESULT__INTERRUPT_STATUS_GPIO);
    status = status & 0x07;
    HAL_Delay(1);
  }

  HAL_Delay(10);
  distance = VL6180X_ReadByte(RESULT__RANGE_VAL);
  VL6180X_WR_CMD(SYSTEM__INTERRUPT_CLEAR, 0X01);
  return distance;
}

// void Vl6180_Init(void)
// {
//   uint8_t reset = 0, timeOut = 0, data_reg;

//   while (!(VL6180X_CheckID() == 0xB4))
//     ;
//   delay_ms(5);

//   VL6180X_WR_CMD(VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x01);
//   while (reset != 1 && timeOut < 100)
//   {
//     reset =  VL6180X_ReadByte(VL6180X_SYSTEM_FRESH_OUT_OF_RESET); // read fresh_out_of_reset bit
//     if (reset == 1)
//     { // if if fresh_out_of_reset bit set, then device has been freshly initialized

//       // SR03 settings AN4545 24/27 DocID026571 Rev 19 SR03 settings
//       // http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
//       // Below are the recommended settings required to be loaded onto the VL6180X during the
//       // initialisation of the device (see Section 1.3).
//       // Mandatory : private registers

//       VL6180X_WR_CMD(0x0207, 0x01);

//       data_reg =  VL6180X_ReadByte(0x0207);

//       VL6180X_WR_CMD(0x0208, 0x01);
//       VL6180X_WR_CMD(0x0096, 0x00);
//       VL6180X_WR_CMD(0x0097, 0xFD);
//       VL6180X_WR_CMD(0x00e3, 0x00);
//       VL6180X_WR_CMD(0x00e4, 0x04);

//       data_reg =  VL6180X_ReadByte(0x00e4);

//       VL6180X_WR_CMD(0x00e5, 0x02);
//       VL6180X_WR_CMD(0x00e6, 0x01);
//       VL6180X_WR_CMD(0x00e7, 0x03);
//       VL6180X_WR_CMD(0x00f5, 0x02);
//       VL6180X_WR_CMD(0x00d9, 0x05);
//       VL6180X_WR_CMD(0x00db, 0xce);
//       VL6180X_WR_CMD(0x00dc, 0x03);
//       VL6180X_WR_CMD(0x00dd, 0xf8);
//       VL6180X_WR_CMD(0x009f, 0x00);
//       VL6180X_WR_CMD(0x00a3, 0x3c);
//       VL6180X_WR_CMD(0x00b7, 0x00);
//       VL6180X_WR_CMD(0x00bb, 0x3c);
//       VL6180X_WR_CMD(0x00b2, 0x09);
//       VL6180X_WR_CMD(0x00ca, 0x09);
//       VL6180X_WR_CMD(0x0198, 0x01);
//       VL6180X_WR_CMD(0x01b0, 0x17);
//       VL6180X_WR_CMD(0x01ad, 0x00);
//       VL6180X_WR_CMD(0x00ff, 0x05);
//       VL6180X_WR_CMD(0x0100, 0x05);
//       VL6180X_WR_CMD(0x0199, 0x05);
//       VL6180X_WR_CMD(0x01a6, 0x1b);
//       VL6180X_WR_CMD(0x01ac, 0x3e);
//       VL6180X_WR_CMD(0x01a7, 0x1f);
//       VL6180X_WR_CMD(0x0030, 0x00);

//       data_reg =  VL6180X_ReadByte(0x0030);

//       // configure range measurement for low power
//       // Specify range measurement interval in units of 10 ms from 0 (= 10 ms) - 254 (= 2.55 s)

//       VL6180X_WR_CMD(VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x0A); // 100 ms interval in steps of 10 ms
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF);         // sets number of range measurements after which autocalibrate is performed
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01);         // perform temperature calibration of the ranging sensor

//       // Set Early Convergence Estimate for lower power consumption
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32);       // set max convergence time to 50 ms (steps of 1 ms)
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01); // enable (0x01) early convergence estimate
//       // This ECE is calculated as follows:
//       // [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
//       // This is ~123 ms for 50 ms max convergence time and 80% below threshold
//       // This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00);     // set early convergence estimate to 5%
//       VL6180X_WR_CMD(VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x7B); // set early convergence estimate to 5%

//       // Configure ALS
//       VL6180X_WR_CMD(VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A); // set to 100 ms
//       data_reg =  VL6180X_ReadByte(VL6180X_SYSALS_INTERMEASUREMENT_PERIOD);

//       // Following is a 16-bit register with the first MSByte reserved

//       VL6180X_WR_CMD(VL6180X_SYSALS_INTEGRATION_PERIOD, 0x00);
//       VL6180X_WR_CMD(VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x63); // set ALS integration time to 100 ms in steps of 1 ms

//       // The internal readout averaging sample period can be adjusted from 0 to 255. Increasing the sampling
//       // period decreases noise but also reduces the effective max convergence time and increases power consumption:
//       // Effective max convergence time = max convergence time - readout averaging period (see
//       // Section 2.5: Range timing). Each unit sample period corresponds to around 64.5 ?s additional
//       // processing time. The recommended setting is 48 which equates to around 4.3 ms
//       VL6180X_WR_CMD(VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30); // compromise between low noise and increased execution time
//       // Gain can be 0 = 20, 1 = 10, 2 = 5, 3 = 2.5, 4 = 1.67, 5 = 1.25, 6 = 1.0 and 7 = 40
//       // These are value possible for the lower nibble. The upper nibble must be 4
//       VL6180X_WR_CMD(VL6180X_SYSALS_ANALOGUE_GAIN, 0x40 | 1); // Sets light and dark gain (don't change upper nibble)
//       data_reg =  VL6180X_ReadByte(VL6180X_SYSALS_ANALOGUE_GAIN);

//       // Scalar (1 - 32, bits 4:0) to multiply raw ALS count for additonal gain, if necessary
//       VL6180X_WR_CMD(VL6180X_FIRMWARE_RESULT_SCALER, 0x01);

//       // Configure the interrupts
//       VL6180X_WR_CMD(VL6180X_SYSTEM_MODE_GPIO0, 0x00);            // set up GPIO 0 (set to high impedence for now)
//       VL6180X_WR_CMD(VL6180X_SYSTEM_MODE_GPIO1, 0x00);            // set up GPIO 1 (set to high impedence for now)
//       VL6180X_WR_CMD(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24); // enable sample ready interrupt

//       // Clear reset bit
//       VL6180X_WR_CMD(VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00); // reset fresh_out_of_reset bit to zero
//     }
//     else
//     {
//       timeOut++;
//     }
//   }
//   reset = 0;
// }
