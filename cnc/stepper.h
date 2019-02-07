#ifndef stepper_h
#include <stdint.h>
#include "config.h"


#define JOG_KEY_LEFT 1
#define JOG_KEY_RIGHT 2


#define TMR_REP_ST_POSITION 0
#define TMR_REP_TASK_RUNNING_STATE 1
#define TMR_REP_NC_MODE_STATE 2
#define TMR_COUNT 3


#define ST_MODE_STOPPED 0
#define ST_MODE_ACC 1
#define ST_MODE_CRUISE 2
#define ST_MODE_DEC 3
#define ST_MODE_RUNNING 4


#define NC_MODE_STOPPED 0
#define NC_MODE_AUTO 1
#define NC_MODE_JOG 2
#define NC_MODE_MPG 3
#define NC_MODE_HOME 4


struct frep_time_t{
  uint32_t t;
  uint32_t dt;
  };


struct stepper_params_t{
  uint8_t  stepbits_invert_mask;
  uint16_t rate_min;
  uint16_t rate_max[NAXIS];
  uint32_t st_inv_acc;
  uint16_t tmr_dt[TMR_COUNT];
  uint32_t position[NAXIS];
  };



struct task_t{
  uint8_t id;
  uint8_t dirbits;
  int16_t err[NAXIS];
  int16_t step[NAXIS]; // axis
  int16_t time; //time
  uint16_t steps; //step_count
  uint16_t steps_dec;
  uint16_t rate;
  };


void on_steppers_disable();
void on_steppers_enable();
void on_nc_mode_change(uint8_t new_mode);
void on_axis(uint8_t ax);
void on_jog_rate_max(uint16_t rate_max);


void stepper_init();
void stepper_stop();
void stepper_loop();

task_t* task_reserve_cell();
void task_apply(task_t* t);
void tasks_execute();
void report_task_running_state(uint32_t ms_time);
void report_stepper_position(uint32_t ms_time);

uint32_t stepper_position(uint8_t ax);

void jog_key_click(uint8_t key);


extern stepper_params_t params;

#define stepper_h
#endif
