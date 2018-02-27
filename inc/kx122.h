#ifndef __KX122_H
#define __KX122_H

#include "stm32f30x.h"

void kx122_init(void);
uint8_t kx122_read(uint8_t addr);
void kx122_write(uint8_t addr, uint8_t data);

#endif
