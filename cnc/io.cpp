#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "config.h"
#include "io.h"

volatile uint16_t isr_pin_state;


ISR(PCINT0_vect){ //PORTB
  isr_pin_state = ((uint16_t)PINB) | (isr_pin_state & 0xFF00);
  //sei();
  
  }//isr pcint1


ISR(PCINT2_vect){ //A8..A15
  isr_pin_state = (((uint16_t)PINK) << 8) | (isr_pin_state & 0x00FF);
  //sei();
  
  }//isr pcint2


void io_init(){
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
  DDRB = DDRK = 0;
  PCMSK0 = PCMSK2 = PORTK = PORTB = 255;
  PCICR |= (1 << PCIE2) | (1 << PCIE0);
#else
#endif  
  }//void
