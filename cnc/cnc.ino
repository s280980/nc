#include <avr/io.h>
#include <avr/sleep.h>
#include "serial.h"
#include "protocol.h"
#include "stepper.h"


void setup(){
  serial_init();
  stepper_init();
  params.frep__task_running_state__dt = 1000;  
  }//setup



void loop(){
  //sleep_mode();
  protocol_process_input();
  task_running_state(millis());
  }//loop
