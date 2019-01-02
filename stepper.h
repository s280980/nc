#ifndef stepper_h
#include <stdint.h>
#include "config.h"

#define ST_MODE_STOPPED 0
#define ST_MODE_ACC 1
#define ST_MODE_CRUISE 2
#define ST_MODE_DEC 3


struct task_t{
  uint8_t id;
  uint8_t dirbits;
  uint16_t err[NAXIS];
  uint16_t step[NAXIS]; // axis
  uint16_t time; //time
  uint16_t rate;
  uint16_t steps; //step_count
  uint16_t steps_acc;
  uint16_t steps_dec;
  };


void stepper_init();
void tasks_execute();
void stepper_stop();



#define stepper_h
#endif
