//---------------------------------------------------------------------------

#ifndef RestarterFrmH
#define RestarterFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
//---------------------------------------------------------------------------
class TRestarterForm : public TForm
{
__published:	// IDE-managed Components
	TIdDecoderMIME *IdDecoderMIME;
	TTimer *Timer;
	TTimer *ProcessTimer;
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ProcessTimerTimer(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TRestarterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
#endif
