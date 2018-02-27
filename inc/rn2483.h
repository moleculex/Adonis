#ifndef __RN2483_H
#define __RN2483_H

#define	RN2483_INIT 0x01
#define	RN2483_TX 0x02

typedef struct{
	char rxBuffer[128];
	char txBuffer[128];
	char *rxBuffer_ptr;
	char deveui[16];
}t_rn2483;

t_rn2483 _rn2483;

void rn2483_init(void);
void rn2483_join(void);
void rn2483_sleep(void);
void rn2483_write(char *buffer, char length);

#endif
