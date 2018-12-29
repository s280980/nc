#ifndef serial_h

//#define SERIAL_ISR_OFF


#define SERIAL_RATE 500000 //or low rate = 115200
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
#define SERIAL_RX_BUFFER_SIZE 512
#else
#define SERIAL_RX_BUFFER_SIZE 64
#endif
#define SERIAL_TX_BUFFER_SIZE 64

void serial_init();
uint8_t serial_read();
void serial_write(uint8_t data);
bool serial();

#define serial_h
#endif
