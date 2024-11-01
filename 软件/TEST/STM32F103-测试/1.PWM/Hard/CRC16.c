#include "CRC16.h"

uint16_t CRC16_calculate(uint8_t *puchMsg, int usDataLen)
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
