#include <stdint.h>
#include <avr/interrupt.h>
#include "config.h"
#include "stepper.h"


struct stepper_params_t{
  uint8_t  stepbits_invert_mask;
  uint16_t rate_min;
  uint32_t st_inv_acc;
  };
static stepper_params_t params;  


volatile uint16_t st_rate;
static volatile uint16_t st_rate_start;
static volatile uint8_t st_stepbits;
static volatile uint32_t st_dtime;
static volatile uint32_t st_time;
volatile uint32_t st_position[NAXIS];
uint8_t st_mode;


task_t tasks[TASK_BUFFER_SIZE];
task_t *task;
uint8_t task_head=0;
volatile uint8_t task_tail=0;


inline stepper_stepbits_update(){
  st_stepbits=0;
  uint8_t bt;
  for(uint8_t ax=0;ax<NAXIS;ax++){
    task->err[ax]+=task->step[ax];
    if(2*task->err[ax]>=task->time){
      task->err[ax]-=task->time;
      bt=(1<<ax);
      st_stepbits|=bt;
      if(task->dirbits&bt){st_position[ax]++;}else{st_position[ax]--;}
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
  //stepper_synchronize();
  TIMSK1 &= ~(1<<OCIE1A);
  STEPPERS_DISABLE_PORT |= 1<<STEPPERS_DISABLE_BIT;  
  st_rate = 0;
  st_mode = ST_MODE_STOPPED;
  }

  
void stepper_start(){
  STEPPERS_DISABLE_PORT &= ~(1<<STEPPERS_DISABLE_BIT);
  TIMSK1 |= (1<<OCIE1A);  
  }


inline stepper_ocr1a_update(){
  //math rate...
  if(task->steps>task->steps_acc){
    if(st_mode!=ST_MODE_ACC){st_time=0; st_rate_start = st_rate; st_mode=ST_MODE_ACC;}
    st_time += st_dtime;
    st_rate = st_rate_start + st_time / params.st_inv_acc;
    st_dtime = 250000 / st_rate;
    OCR1A = st_dtime;
    }
  else if(task->steps<task->steps_dec){
    if(st_mode!=ST_MODE_DEC){st_time=0; st_rate_start = st_rate; st_mode=ST_MODE_DEC;}
    st_time += st_dtime;
    uint16_t tmp = st_time / params.st_inv_acc;
    if(st_rate_start-4>=tmp){st_rate = st_rate_start - tmp;}else{st_rate = 4;}
    st_dtime = 250000 / st_rate;
    OCR1A = st_dtime;
    }else{
      if(st_mode!=ST_MODE_CRUISE){
        st_mode=ST_MODE_CRUISE;
        st_dtime = 250000 / st_rate;
        OCR1A = st_dtime;
        }
      }      
  }//inline ocr1a_update


void tasks_execute(){
  st_rate = params.rate_min;
  stepper_task_update();
  if(task){
    stepper_stepbits_update();
    stepper_ocr1a_update();
    stepper_start();
    }
  }//void task_execute


ISR(TIMER1_COMPA_vect){
  STEP_PORT = st_stepbits;
  sei();
  TCNT2=0;  TCCR2B = (1<<CS01);  
  stepper_task_update();
  if(task){    
    stepper_stepbits_update();    
    stepper_ocr1a_update();
  }else{
    stepper_stop();
    }
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

  }//stepper_init


void stepper_flush(){
  cli();
  //
  sei();
  }  

