#include <stdint.h>
#include "config.h"
#include "serial.h"
#include "protocol.h"
#include "stepper.h"


uint8_t pr_buffer[PROTOCOL_RX_BUFFER_SIZE];
uint8_t pr_bytes_read = 0;
uint16_t pr_command = 0;
uint8_t pr_bytes_wait = 0;


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
        //case CMD_TASK:{pr_bytes_wait=}break;
        
        }//switch
      }
    else if(pr_bytes_wait>0){
      pr_buffer[pr_bytes_read++]=data;
      pr_bytes_wait--;
      if(pr_bytes_wait==0){
        switch(pr_command){
          //case CMD_TASK:{}break;
          case CMD_TASK_RUNNING_STATE_REP_DT_SET:{ params.tmr_dt[1] = (((uint16_t)pr_buffer[0])<<7) | pr_buffer[1]; }break;
          case CMD_STEPPER_POSITION_REP_DT_SET:{ params.tmr_dt[0] = (((uint16_t)pr_buffer[0])<<7) | pr_buffer[1]; }break;
          
          //case (CMD_TASK + 256):{}break;
          }//switch cmd
        }
      }  
    }//serial
  }//void
