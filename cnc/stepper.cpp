#include <stdint.h>
#include <avr/interrupt.h>
#include "config.h"
#include "stepper.h"
#include "serial.h"
#include "protocol.h"
#include "eeprom.h"



static stepper_params_t params;  
uint32_t tmr_time[TMR_COUNT];

volatile uint16_t st_rate;
static volatile uint16_t st_rate_start;
static volatile uint8_t st_stepbits;
static volatile uint32_t st_dtime;
static volatile uint32_t st_time;
volatile uint32_t st_position[NAXIS];
uint8_t st_mode;
uint8_t nc_mode;


task_t tasks[TASK_BUFFER_SIZE];
task_t *task;
uint8_t task_head=0;
volatile uint8_t task_tail=0;


uint32_t stepper_position(uint8_t ax){
  return st_position[ax];
  }


uint32_t rep_st_position[NAXIS];
void report_stepper_position(uint32_t ms_time){
  if(!params.tmr_dt[TMR_REP_ST_POSITION]){return; }
  if(ms_time >= tmr_time[TMR_REP_ST_POSITION] + params.tmr_dt[TMR_REP_ST_POSITION]){
    uint8_t data;
    tmr_time[TMR_REP_ST_POSITION] = ms_time;
    for(uint8_t ax=0; ax<NAXIS; ax++){
      if(rep_st_position[ax] != st_position[ax]){
        rep_st_position[ax] = st_position[ax];
        serial_write( CMD_STEPPER_POSITION );
        data = rep_st_position[ax]>>28;
        data |= (ax<<4);
        serial_write( data &127 );
        serial_write( (rep_st_position[ax]>>21) &127 );
        serial_write( (rep_st_position[ax]>>14) &127 );
        serial_write( (rep_st_position[ax]>>7) &127 );
        serial_write( rep_st_position[ax] &127 );
        }
      }
    }
  }//void


uint8_t rep_task_id;
int16_t rep_task_steps;
void report_task_running_state(uint32_t ms_time){
  if(!params.tmr_dt[TMR_REP_TASK_RUNNING_STATE]){ return; }
  task_t *t;
  t=task;
  if(t && (ms_time - tmr_time[TMR_REP_TASK_RUNNING_STATE] >= params.tmr_dt[TMR_REP_TASK_RUNNING_STATE])){
    tmr_time[TMR_REP_TASK_RUNNING_STATE] = ms_time;
    if((t->id!=rep_task_id)||(t->steps!=rep_task_steps)){
      rep_task_id = t->id;
      rep_task_steps = t->steps;
      serial_write(CMD_TASK_RUNNING_STATE);
      serial_write( (rep_task_id>>2) &127 );
      serial_write( ((rep_task_id<<2) || (rep_task_steps>>14)) &127 );
      serial_write( (rep_task_steps>>7) &127 );
      serial_write( (rep_task_steps) &127);
      }//if !=id
    }
  }//void


void on_nc_mode_change(uint8_t new_mode){
  if(st_rate != 0){return;}
  nc_mode = new_mode;
  serial_write(CMD_MODE_STATE);
  serial_write(nc_mode &127);
  }//void


task_t* task_reserve_cell(){
  uint8_t head = task_head+1;
  head%=TASK_BUFFER_SIZE;
  if(head!=task_tail){return &tasks[head];}else{return 0;}
  }//task_t* ..()


void task_apply(task_t* t){
  uint8_t head = task_head+1;
  head%=TASK_BUFFER_SIZE;
  if(&tasks[head]==t){task_head = head;}
  }


inline stepper_stepbits_update(){
  st_stepbits=0;
  uint8_t bt;
  for(uint8_t ax=0;ax<NAXIS;ax++){
    task->err[ax]+=task->step[ax];
    if(2*task->err[ax]>=task->time){
      task->err[ax]-=task->time;
      bt=(1<<ax);
      st_stepbits|=bt;
      if(task->dirbits&bt){st_position[ax]--;}else{st_position[ax]++;}
      }//if
    }//for  
  st_stepbits ^= params.stepbits_invert_mask;
  task->steps--;
  if(task->steps==0){task=0;}  
  }//inline stepbits_update


inline stepper_task_update(){
  if(task==0){
    if(task_head!=task_tail){
      task_tail = (task_tail+1) % TASK_BUFFER_SIZE;
      task = &tasks[task_tail];
      DIRECTION_PORT = task->dirbits;      
      }
    }//if task==0
  }//inline task_update()


void stepper_stop(){
  TIMSK1 &= ~(1<<OCIE1A);  
  st_rate = 0;
  st_mode = ST_MODE_STOPPED;
  }

  
void stepper_start(){
  st_mode = ST_MODE_RUNNING;
  TIMSK1 |= (1<<OCIE1A);  
  }


void on_steppers_enable(){
  STEPPERS_DISABLE_PORT &= ~(1<<STEPPERS_DISABLE_BIT);
  serial_write(CMD_STEPPERS_STATE);
  serial_write(1);
  }


void on_steppers_disable(){
  STEPPERS_DISABLE_PORT |= 1<<STEPPERS_DISABLE_BIT;  
  serial_write(CMD_STEPPERS_STATE);
  serial_write(0);
  }


inline stepper_ocr1a_update(){
  //math rate...
  if(task->steps<task->steps_dec){
    if(st_mode!=ST_MODE_DEC){st_time=0; st_rate_start = st_rate; st_mode=ST_MODE_DEC;}
    st_time += st_dtime;
    uint16_t tmp = st_time / params.st_inv_acc;
    if(st_rate_start-4>=tmp){st_rate = st_rate_start - tmp;}else{st_rate = 4;}
    st_dtime = 250000 / st_rate;
    OCR1A = st_dtime;
    }
  else if(st_rate<task->rate){
    if(st_mode!=ST_MODE_ACC){st_time=0; st_rate_start = st_rate; st_mode=ST_MODE_ACC;}
    st_time += st_dtime;
    st_rate = st_rate_start + st_time / params.st_inv_acc;
    st_dtime = 250000 / st_rate;
    OCR1A = st_dtime;
    }
  else{
      if(st_mode!=ST_MODE_CRUISE){
        st_mode=ST_MODE_CRUISE;
        st_dtime = 250000 / st_rate;
        OCR1A = st_dtime;
        }
      }      
  }//inline ocr1a_update


void tasks_execute(){
  if(nc_mode != NC_MODE_AUTO){return;}
  st_rate = params.rate_min;
  stepper_task_update();
  if(task){
    stepper_stepbits_update();
    stepper_ocr1a_update();
    stepper_start();
    }
  }//void task_execute


ISR(TIMER1_COMPA_vect){
  switch(nc_mode){
    case NC_MODE_AUTO:{
      STEP_PORT = st_stepbits;
      sei();
      TCNT2=0;  TCCR2B = (1<<CS01);
      stepper_task_update();
      if(task){
        stepper_stepbits_update();
        stepper_ocr1a_update();
        }else{stepper_stop();}
      }break;
    case NC_MODE_JOG:{}break;
    case NC_MODE_MPG:{}break;
    case NC_MODE_HOME:{}break;  
    }//switch
  }//ISR


ISR(TIMER2_COMPA_vect){
  }

  
ISR(TIMER2_OVF_vect){
  TCCR2B = 0;//disable timer
  STEP_PORT = params.stepbits_invert_mask;  
  }

void stepper_init(){
  // Configure step and direction interface pins
  STEP_DDR |= STEP_MASK;
  STEPPERS_DISABLE_DDR |= 1<<STEPPERS_DISABLE_BIT;
  DIRECTION_DDR |= DIRECTION_MASK;

  // Configure Timer 1: Stepper Driver Interrupt
  TIMSK1 &= ~(1 << OCIE1A);
  TCCR1A = 0;
  TCCR1B = 0;  
  OCR1A = 62500;
  TCCR1B |= (1<<WGM12);
  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (3<<CS10);
  
  // Configure Timer 2
  TIMSK2 = 0; // All interrupts disabled
  TCCR2A = 0; // Normal operation  
  TCCR2B = 1<<CS20; // Full speed, no prescaler
  TIMSK2 |= (1<<TOIE2); //enable  TIMER2_OVF_vect


  if(!memcpy_from_eeprom_with_checksum((uint8_t*)&params,0,sizeof(stepper_params_t))){
    params.st_inv_acc = 100;
    params.rate_min = 50;
    for(uint8_t ax=0;ax<NAXIS;ax++){st_position[ax]=0;}
    st_position[3]=17547;
    st_position[1]=3452175477;
    }

  st_mode = ST_MODE_STOPPED;
  nc_mode = NC_MODE_STOPPED;
    
  }//stepper_init


void stepper_flush(){
  cli();
  //
  sei();
  }  
