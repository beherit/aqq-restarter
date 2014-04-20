//---------------------------------------------------------------------------

#ifndef RestarterFrmH
#define RestarterFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TRestarterForm : public TForm
{
__published:	// IDE-managed Components
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TRestarterForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
#endif
