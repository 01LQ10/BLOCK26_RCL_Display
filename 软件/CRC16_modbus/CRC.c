#include <stdio.h>
#include <stdbool.h>
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef int int32_t;
const uint16_t polynom = 0x8005;

//uint16_t calculate_crc16(uint16_t wCRCin, uint16_t wCPoly, uint16_t wResultXOR, 
//						bool input_invert, bool ouput_invert, uint8_t *puchMsg, int usDataLen)
//{
//    uint8_t wChar = 0;
//    while (usDataLen--)
//    {
//        wChar = *(puchMsg++);
//        if (input_invert) // 输入值反转
//        {
//            uint8_t temp_char = wChar;
//            wChar = 0;
//            for (int i = 0; i < 8; ++i)
//            {
//                if (temp_char & 0x01)
//                    wChar |= 0x01 << (7 - i);
//                temp_char >>= 1;
//            }
//        }
//        wCRCin ^= (wChar << 8);
//        for (int i = 0; i < 8; i++)
//        {
//            if (wCRCin & 0x8000)
//                wCRCin = (wCRCin << 1) ^ wCPoly;
//            else
//                wCRCin = wCRCin << 1;
//        }
//    }
//    if (ouput_invert)
//    {
//        uint16_t temp_short = wCRCin;
//        wCRCin = 0;
//        for (int i = 0; i < 16; ++i)
//        {
//            if (temp_short & 0x01)
//                wCRCin |= 0x01 << (15 - i);
//            temp_short >>= 1;
//        }
//    }
//    return (wCRCin ^ wResultXOR);
//}
//
//uint16_t crc16bitbybit(uint8_t *ptr, uint16_t len)
//{
//    uint8_t i;
//    uint16_t crc = 0xffff;
//
//    if (len == 0)
//    {
//        len = 1;
//    }
//    while (len--)
//    {
//        crc ^= *ptr;
//        for (i = 0; i < 8; i++)
//        {
//            if (crc & 1)
//            {
//                crc >>= 1;
//                crc ^= polynom;
//            }
//            else
//            {
//                crc >>= 1;
//            }
//        }
//        ptr++;
//    }
//    return (crc);
//}

uint16_t calculate_crc16(uint8_t *puchMsg, int usDataLen)
{
	uint16_t wCRCinit = 0xFFFF;
	uint8_t wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);

		uint8_t temp_char = wChar;
		wChar = 0;
		for (int i = 0; i < 8; ++i)
		{
			if (temp_char & 0x01)
				wChar |= 0x01 << (7 - i);
			temp_char >>= 1;
		}

		wCRCinit ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCinit & 0x8000)
				wCRCinit = (wCRCinit << 1) ^ 0x8005;
			else
				wCRCinit = wCRCinit << 1;
		}
	}

	uint16_t temp_short = wCRCinit;
	wCRCinit = 0;
	for (int i = 0; i < 16; ++i)
	{
		if (temp_short & 0x01)
			wCRCinit |= 0x01 << (15 - i);
		temp_short >>= 1;
	}

	return wCRCinit;
}

uint8_t crc16_data[6] = {0x01, 0x03, 0x02, 0x00, 0x15, 0x01};
int main(void)
{
    uint16_t crcout;
    crcout = calculate_crc16(crc16_data, 5);
    printf("crc is :%x", crcout);
    return 0;
}
