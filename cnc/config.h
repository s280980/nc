#define SERIAL_RATE 500000 //or low rate = 115200
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
  #define SERIAL_RX_BUFFER_SIZE 512
#else
  #define SERIAL_RX_BUFFER_SIZE 64
#endif
#define SERIAL_TX_BUFFER_SIZE 64

#define PROTOCOL_RX_BUFFER_SIZE 64
#define TICKS_PER_MICROSECOND ( F_CPU / 1000000 )
#define TASK_BUFFER_SIZE 10

#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega2560__)
  #define NAXIS 8
  #define STEP_DDR DDRA
  #define STEP_PORT PORTA  
  #define DIRECTION_DDR DDRC
  #define DIRECTION_PORT PORTC  
  #define STEPPERS_DISABLE_PORT PORTG
  #define STEPPERS_DISABLE_DDR DDRG
  #define STEPPERS_DISABLE_BIT 0  
  #define STEP_MASK 255
  #define DIRECTION_MASK 255
#else
 
#endif

