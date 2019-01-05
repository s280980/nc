//---------------------------------------------------------------------------

#ifndef ioPortH
#define ioPortH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <stdint.h>
//---------------------------------------------------------------------------
#define BUFSIZE 4096
#define EBUFSIZE 4096
#define PROTOCOL_RX_BUFFER_SIZE 256
//---------------------------------------------------------------------------
#define NAXIS 6
//---------------------------------------------------------------------------
//pc-->nc
#define CMD_TASK 128
#define CMD__FREP__TASK_RUNNING_STATE__MSTIME 135
#define CMD_RESET 136

//nc-->pc
#define CMD_TASK_RUNNING_STATE 135


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class ComPortThread;
//---------------------------------------------------------------------------
#define TASK_COUNT  30
struct task_t{
  uint8_t id;
  uint8_t dirbits;
  int16_t err[NAXIS];
  int16_t step[NAXIS]; // axis
  int16_t time; //time
  int16_t steps; //step_count
  int16_t steps_acc;
  int16_t steps_dec;
  uint16_t rate;
  uint8_t enable;
  };

//---------------------------------------------------------------------------
class ReadThread : public TThread
{
private:
        void __fastcall print();
        DWORD btr;
        unsigned char buf[EBUFSIZE];
protected:
        void __fastcall Execute();
public:
        __fastcall ReadThread(bool CreateSuspended);
        ComPortThread *port;
};
//---------------------------------------------------------------------------
class WriteThread : public TThread
{
private:
        void __fastcall buf_update();
        DWORD buf_used;
        unsigned char buf[EBUFSIZE];
        bool fl;
protected:
        void __fastcall Execute();
public:
        __fastcall WriteThread(bool CreateSuspended);
        ComPortThread *port;
};
//---------------------------------------------------------------------------
class ComPortThread : public TThread
{
private:
protected:
        void __fastcall Execute();
public:
        __fastcall ComPortThread(bool CreateSuspended);
        int __fastcall Open(int numPort, uint32_t boudrate);
        void __fastcall Close();

        HANDLE handle;
        uint8_t rbuf[BUFSIZE], wbuf[BUFSIZE];
        uint16_t rtail, rhead, wtail, whead;
        WriteThread *writer;
        ReadThread *reader;
        uint8_t __fastcall Read();
        uint8_t pr_buffer[PROTOCOL_RX_BUFFER_SIZE];
        uint8_t pr_bytes_read;
        uint16_t pr_command;
        uint8_t pr_bytes_wait;
        void __fastcall Write(uint8_t data);

        task_t tasks[TASK_COUNT];
        uint8_t task_head;
        uint8_t task_tail;
        uint32_t stepper_position[8];

        void __fastcall task_running_state__report_dtime_set(uint16_t ms);



};
//---------------------------------------------------------------------------
#endif
