#include <stdint.h>
#include <avr/interrupt.h>
#include "config.h"
#include "serial.h"

uint8_t rx_buffer[SERIAL_RX_BUFFER_SIZE];
volatile uint8_t rx_buffer_head = 0;
uint8_t rx_buffer_tail = 0;
uint8_t tx_buffer[SERIAL_TX_BUFFER_SIZE];
uint8_t tx_buffer_head = 0;
volatile uint8_t tx_buffer_tail = 0;

bool serial(){
  return rx_buffer_head != rx_buffer_tail;
  }

void serial_reset_rx_tx_buffer(){ rx_buffer_head = rx_buffer_tail = tx_buffer_head = tx_buffer_tail = 0; }

void serial_init(){
  serial_reset_rx_tx_buffer();
  #if SERIAL_RATE < 57600
    uint16_t UBRR0_value = ((F_CPU / (8L * SERIAL_RATE)) - 1)/2 ;
    UCSR0A &= ~(1 << U2X0); // baud doubler off  - Only needed on Uno XXX
  #else
    uint16_t UBRR0_value = ((F_CPU / (4L * SERIAL_RATE)) - 1)/2;
    UCSR0A |= (1 << U2X0);  // baud doubler on for high baud rates, i.e. 115200
  #endif
  UBRR0H = UBRR0_value >> 8;
  UBRR0L = UBRR0_value;  
  // enable rx and tx
  UCSR0B |= 1<<RXEN0;
  UCSR0B |= 1<<TXEN0;
  // enable interrupt on complete reception of a byte
  UCSR0B |= 1<<RXCIE0;
  }
  
uint8_t serial_read()
{
  uint8_t data = rx_buffer[rx_buffer_tail];
  rx_buffer_tail++;
  if (rx_buffer_tail == SERIAL_RX_BUFFER_SIZE) { rx_buffer_tail = 0; } 
  return data; 
}

void serial_write(uint8_t data) {
  uint8_t next_head = tx_buffer_head + 1;
  if (next_head == SERIAL_TX_BUFFER_SIZE) { next_head = 0; }
  while (next_head == tx_buffer_tail) {;}
  tx_buffer[tx_buffer_head] = data;
  tx_buffer_head = next_head;  
  UCSR0B |=  (1 << UDRIE0); 
}

#ifndef SERIAL_ISR_OFF
// serial isr rx
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
ISR(USART0_RX_vect)
#else
ISR(USART_RX_vect)
#endif
{
  uint8_t data = UDR0;
  uint8_t next_head;
  next_head = rx_buffer_head + 1;
  if (next_head == SERIAL_RX_BUFFER_SIZE) { next_head = 0; }  
  if (next_head != rx_buffer_tail) { rx_buffer[rx_buffer_head] = data; rx_buffer_head = next_head; }
}//ISR RX

// serial isr tx
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
ISR(USART0_UDRE_vect)
#else
ISR(USART_UDRE_vect)
#endif
{
  uint8_t tail = tx_buffer_tail;  
  UDR0 = tx_buffer[tail];
  tail++;
  if (tail == SERIAL_TX_BUFFER_SIZE) { tail = 0; }  
  tx_buffer_tail = tail;
  if (tail == tx_buffer_head) { UCSR0B &= ~(1 << UDRIE0); }
}
#endif
