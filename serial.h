#ifndef serial_h
#include <stdint.h>

//#define SERIAL_ISR_OFF
//#define SERIAL_PRINT

void serial_init();
uint8_t serial_read();
void serial_write(uint8_t data);
bool serial();

#ifdef SERIAL_PRINT
  void serial_print(char *s);
  void serial_println(char *s);
  void serial_println(uint32_t n);
#endif

#define serial_h
#endif
