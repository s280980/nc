#include <avr/io.h>
#include <stdint.h>
#include "config.h"
#include "io.h"



void io_init(){
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
  DDRF = DDRK = 0;
  PCMSK1 = PCMSK2 = PORTF = PORTK = 255;
  PCICR |= (1 << PCIE2) | (1 << PCIE1);
#else
#endif  
  }//void
