#ifndef serial_h
#include <stdint.h>

//#define SERIAL_ISR_OFF

void serial_init();
uint8_t serial_read();
void serial_write(uint8_t data);
bool serial();

#define serial_h
#endif
