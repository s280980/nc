#include <stdint.h>
#include "config.h"
#include "serial.h"
#include "protocol.h"
#include "stepper.h"


uint8_t pr_buffer[PROTOCOL_RX_BUFFER_SIZE];
uint8_t pr_bytes_read = 0;
uint16_t pr_command = 0;
uint8_t pr_bytes_wait = 0;


void protocol_process_input(){
  if(serial()){
    uint8_t data = serial_read();
    if(data>127){
      pr_command = data;
      pr_bytes_wait = pr_bytes_read = 0;
      switch(pr_command){
        case CMD_TASK:{/*pr_bytes_wait=;*/}break;
        case CMD__FREP__TASK_RUNNING_STATE__MSTIME:{pr_bytes_wait=2;}break;
        
        }//switch
      }
    else if(pr_bytes_wait>0){
      pr_buffer[pr_bytes_read++]=data;
      pr_bytes_wait--;
      if(pr_bytes_wait==0){
        switch(pr_command){
          case CMD_TASK:{}break;
          case CMD__FREP__TASK_RUNNING_STATE__MSTIME:{ params.frep__task_running_state__dt = (((uint16_t)pr_buffer[1])<<7) || pr_buffer[0]; }break;
          
          case (CMD_TASK + 256):{}break;
          }//switch cmd
        }
      }  
    }//serial
  }//void
