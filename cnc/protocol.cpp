#include <stdint.h>
#include "config.h"
#include "serial.h"
#include "protocol.h"
#include "stepper.h"


uint8_t pr_buffer[PROTOCOL_RX_BUFFER_SIZE];
uint8_t pr_bytes_read = 0;
uint16_t pr_command = 0;
uint8_t pr_bytes_wait = 0;


void on_task(){
  //14bytes
  
  if(pr_bytes_read==10){    
    uint8_t enable = pr_buffer[6]<<5;
    enable |= pr_buffer[7]>>2;
    uint8_t n=0;
    for(uint8_t ax=0;ax<8;ax++){
      if(enable & (1 << ax)){ n++; }
      }
    pr_bytes_wait = n*3;  
    }
  else{
    task_t *task;
    task = task_reserve_cell();
    if(task){
      uint8_t data;
      task->id = pr_buffer[0];
      task->dirbits = (pr_buffer[1]<<1) | (pr_buffer[2]>>6);
      uint16_t steps = ((uint16_t)pr_buffer[2])<<10;
      steps |= ((uint16_t)pr_buffer[3])<<3;
      steps |= ((uint16_t)pr_buffer[4]>>4);
      task->steps = steps;
      task->time = steps;
      steps = ((uint16_t)pr_buffer[4]&15)<<11;
      steps |= ((uint16_t)pr_buffer[5])<<4;
      steps |= ((uint16_t)pr_buffer[6]>>3);
      task->steps_dec = steps;
      uint8_t enable = pr_buffer[6]<<5;
      enable |= pr_buffer[7]>>2;
      task-rate = ((uint16_t)pr_buffer[7])<<14;
      task->rate |= ((uint16_t)pr_buffer[8])<<7;
      task->rate |= ((uint16_t)pr_buffer[9]);
      uint8_t num=10;
      while(num<pr_bytes_read){
        for(uint8_t ax=0;ax<NAXIS;ax++){
          task->err[ax]=0;
          if((1<<ax)&enable){
            task->step[ax] = ((uint16_t)pr_buffer[num++])<<14;
            task->step[ax] |= ((uint16_t)pr_buffer[num++])<<7;
            task->step[ax] |= ((uint16_t)pr_buffer[num++]);
            }else{task->step[ax]=0;}
          }
        }//while
      //task_apply(task);
      if(num==pr_bytes_read){
        serial_write( CMD_TASK_ACCEPTED );
        serial_write( task->id &127 );
        }
            
      }//if task reserve
    }    
  }//void



void on_reset(){
  }


void protocol_process_input(){
  if(serial()){
    uint8_t data = serial_read();
    if(data>127){
      pr_command = data;
      pr_bytes_wait = pr_bytes_read = 0;
      switch(pr_command){
        case CMD_LINK:{serial_write(CMD_LINK);}break;
        case CMD_STEPPER_POSITION_REP_DT_SET:
        case CMD_TASK_RUNNING_STATE_REP_DT_SET:{pr_bytes_wait=2;}break;
        case CMD_RESET:{on_reset();}break;
        case CMD_TASK:{pr_bytes_wait=10;}break;
        
        }//switch
      }
    else if(pr_bytes_wait>0){
      pr_buffer[pr_bytes_read++]=data;
      pr_bytes_wait--;
      if(pr_bytes_wait==0){
        switch(pr_command){
          case CMD_TASK:{on_task();}break;
          case CMD_TASK_RUNNING_STATE_REP_DT_SET:{ params.tmr_dt[TMR_REP_TASK_RUNNING_STATE] = (((uint16_t)pr_buffer[0])<<7) | pr_buffer[1]; }break;
          case CMD_STEPPER_POSITION_REP_DT_SET:{ params.tmr_dt[TMR_REP_ST_POSITION] = (((uint16_t)pr_buffer[0])<<7) | pr_buffer[1]; }break;
          
          //case (CMD_TASK + 256):{}break;
          }//switch cmd
        }
      }  
    }//serial
  }//void
