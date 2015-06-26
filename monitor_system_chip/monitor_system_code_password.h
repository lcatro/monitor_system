
#ifndef _MONITOR_SYSTEM_CODE_PASSWORD_H__
#define _MONITOR_SYSTEM_CODE_PASSWORD_H__

#define ADDRESS_START_PASSWORD 0x10

#define LEN_PASSWORD 		   0x10

#define DATA_NULL              0xFF

void password_clean(void) {
	AT24C02_writeData(ADDRESS_START_PASSWORD,LEN_PASSWORD,DATA_NULL);
}

void password_write(unsigned char *password,int len_password) {
	int i=0;
	if (LEN_PASSWORD<len_password)
		return;

	password_clean();
	AT24C02_writeData(ADDRESS_START_PASSWORD,len_password,password);
}

void password_read(unsigned char *out_password,int *out_len_password) {
	unsigned char get_data_bit=DATA_NULL;
	int i=0;
	for (;i<LEN_PASSWORD;i++) {
		AT24C02_readData(ADDRESS_START_PASSWORD+i,1,&get_data_bit);
		if (DATA_NULL==get_data_bit)
			break;
		*(out_password+i)=get_data_bit;
	}
	*out_len_password=i;
}

#endif