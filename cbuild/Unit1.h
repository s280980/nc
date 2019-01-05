//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <stdint.h>
#include "ioPort.h"
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TStatusBar *StatusBar1;
        TMainMenu *MainMenu1;
        TMemo *Memo1;
        TMenuItem *N1;
        TMenuItem *PC1;
        TMenuItem *ARDUINO1;
        TMenuItem *PMC1;
        TMenuItem *N2;
        TMenuItem *HOME1;
        TMenuItem *EDIT1;
        TMenuItem *JOG1;
        TMenuItem *MPG1;
        TMenuItem *AUTO1;
        TTimer *Timer1;
        TButton *Button1;
        TButton *Button2;
        TEdit *Edit2;
        TLabel *Label1;
        TButton *Button5;
        TEdit *Edit3;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Edit2Change(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        ComPortThread *port;
        bool fClose;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
