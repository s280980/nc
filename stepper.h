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
  int16_t err[NAXIS];
  int16_t step[NAXIS]; // axis
  int16_t time; //time
  int16_t steps; //step_count
  int16_t steps_acc;
  int16_t steps_dec;
  };


void stepper_init();
void stepper_stop();


task_t* task_reserve_cell();
void task_apply(task_t* t);
void tasks_execute();


#define stepper_h
#endif
