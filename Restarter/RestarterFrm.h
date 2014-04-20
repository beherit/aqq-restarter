//---------------------------------------------------------------------------
#ifndef RestarterFrmH
#define RestarterFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TRestarterForm : public TForm
{
__published:	// IDE-managed Components
	TIdDecoderMIME *IdDecoderMIME;
	TTimer *Timer;
	TTimer *ProcessTimer;
	TTimer *HideTimer;
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ProcessTimerTimer(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall HideTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TRestarterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
#endif
