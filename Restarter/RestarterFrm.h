//---------------------------------------------------------------------------

#ifndef RestarterFrmH
#define RestarterFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "IdCoderMIME.hpp"
#include "IdCoderUUE.hpp"
#include "IdCoderXXE.hpp"
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
//---------------------------------------------------------------------------
class TRestarterForm : public TForm
{
__published:	// IDE-managed Components
        TTimer *Timer;
        TIdDecoderMIME *IdDecoderMIME;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TRestarterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
#endif
