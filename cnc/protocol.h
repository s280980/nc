#ifndef protocol_h


//pc-->nc
#define CMD_TASK 128
#define CMD__FREP__TASK_RUNNING_STATE__MSTIME 135


//nc-->pc
#define CMD_TASK_RUNNING_STATE 135


void protocol_process_input();


#define protocol_h
#endif
