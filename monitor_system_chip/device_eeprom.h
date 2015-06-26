
#ifndef _DEVICE_EEPROM_H__
#define _DEVICE_EEPROM_H__

void AT24C02_init();
void AT24C02_writeData(unsigned char address,unsigned char numBytes,unsigned char* buf);
void AT24C02_readData(unsigned char beginAddr,unsigned char dataSize,unsigned char* buf);

#endif