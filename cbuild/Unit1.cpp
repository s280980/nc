//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "ioPort.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner), port(0), fClose(0)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  port = new ComPortThread(true);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormShow(TObject *Sender)
{
  uint8_t numPort = 8;
  StatusBar1->Panels->Items[0]->Text = "COM"+String(numPort);

  if(port && (port->handle==0)){

    if(port->Open(numPort,115200)>0){
      StatusBar1->Panels->Items[1]->Text = "OPEN";
      for(int i=0; i<1000; i++){ Application->ProcessMessages(); }
      port->Write( CMD_RESET );
      port->Write( CMD_LINK );
      port->task_running_state__rep_dt_set(1000);
      port->stepper_position__rep_dt_set(1000);
    }
    else{
      StatusBar1->Panels->Items[1]->Text = "CLOSE";
    }
  }//if port
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(port){port->Close();port->Terminate();}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  fClose=true;
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = fClose;
  Timer1->Enabled = true;
  if(!fClose){
    if(port){
    port->Write( CMD_RESET );
    port->stepper_position__rep_dt_set(0);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
  if(port && (port->handle)){
    task_t t;
    t.id = 12;
    t.step[0] = 100;
    t.rate = 2699;
    t.dirbits = 1;
    t.steps = 10000;
    t.time = 10000;
//    port->SendTask(&t);
  }//
}
//---------------------------------------------------------------------------















void __fastcall TForm1::Edit2Change(TObject *Sender)
{
  String s = Edit2->Text;
  if(s==""){s="1000";}
  uint16_t acc = s.ToInt();
  if (acc<10) {acc=10;}
  Form1->Memo1->Lines->Add("<<acc="+String(acc));
  //port->acc_set(acc);

}
//---------------------------------------------------------------------------







