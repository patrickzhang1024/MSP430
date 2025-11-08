#include "driverlib.h"
#include "Board.h"
#include "crc.h"

uint16_t crc16_ccitt_false(uint8_t* buf, uint32_t size)
{
    CRC_setSeed(CRC_BASE, 0xFFFF);
    uint8_t i = 0;
    for (i = 0; i < size; i++)
    {
        CRC_set8BitDataReversed(CRC_BASE, buf[i]);
    }

    return CRC_getResult(CRC_BASE);
}

// This is software function for CRC
/*unsigned short CRC16_CCITT_FALSE(unsigned char *puchMsg, unsigned int usDataLen)
{

    unsigned short wCRCin = 0xFFFF;
    unsigned short wCPoly = 0x1021;
    unsigned char wChar = 0;

    int i = 0;
    while (usDataLen--)
    {
        wChar = *(puchMsg++);
        wCRCin ^= (wChar << 8);
        for(i = 0;i < 8;i++)
        {
            if(wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }

    return (wCRCin) ;
}*/


