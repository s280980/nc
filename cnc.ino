#include <avr/io.h>
#include <avr/sleep.h>
#include "serial.h"
#include "protocol.h"
#include "stepper.h"

void setup(){
  serial_init();
  stepper_init();
  }
  
void loop(){
  //sleep_mode();
  protocol_process_input();
  }
