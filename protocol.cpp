#include <stdint.h>
#include "config.h"
#include "serial.h"
#include "protocol.h"


uint8_t pr_buffer[PROTOCOL_RX_BUFFER_SIZE];
uint8_t pr_read = 0;
uint16_t pr_cmd = 0;
uint8_t pr_wait = 0;


void protocol_process_input(){
  if(serial()){
    uint8_t data = serial_read();
    if(data>127){
      pr_cmd = data;
      pr_wait = pr_read = 0;
      switch(pr_cmd){
        
        }//switch
      }
    else if(pr_wait>0){
      pr_buffer[pr_read++]=data;
      pr_wait--;
      if(pr_wait==0){
        switch(pr_cmd){
          
          }//switch cmd
        }
      }  
    }//serial
  }//void
