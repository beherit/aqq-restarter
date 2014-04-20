//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RestarterFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdCoderMIME"
#pragma resource "*.dfm"
TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
__fastcall TRestarterForm::TRestarterForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

AnsiString Password;
//AnsiString Profile;
int Count=0;

void __fastcall TRestarterForm::FormShow(TObject *Sender)
{
  Application->Minimize();
  ShowWindow(Application->Handle, SW_HIDE);

  //Odczyt sciezki AQQ, has³a i nazwy profilu
  TIniFile *Ini = new TIniFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");
  AnsiString AQQPath = Ini->ReadString("Restarter", "AQQPath", "");
  Password = Ini->ReadString("Restarter", "Password", "");
  //Profile = Ini->ReadString("Restarter", "Profile", "");
  delete Ini;
  //Usuwanie pliku INI
  if(FileExists(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini"))
   DeleteFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");

  AQQPath=StringReplace(AQQPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  Password=IdDecoderMIME->DecodeToString(Password);

  UTF8String PasswordUTF8 = Password;
  Password=Utf8ToAnsi(PasswordUTF8);

  if(AQQPath!="")
  {
    //W³aczenie AQQ
    ShellExecute(NULL, "open", AQQPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    Timer->Enabled=true;
  }
  else
   Close();
}
//---------------------------------------------------------------------------
void __fastcall TRestarterForm::TimerTimer(TObject *Sender)
{
  HWND AQQ = FindWindow("TfrmLogon",NULL);
  if(AQQ!=NULL)
  {
    //AQQ = FindWindowEx(AQQ,NULL,"TComboBox",NULL);

    //if(AQQ!=NULL)
    //{
    //  int index = SendMessage(AQQ, CB_SELECTSTRING,-1,(LPARAM)Profile.c_str());
    //  SendMessage(AQQ, CB_SETCURSEL,index,0);

      //AQQ = FindWindow("TfrmLogon",NULL);
      AQQ = FindWindowEx(AQQ,NULL,"TEdit",NULL);
      if(AQQ!=NULL)
      {
        SendMessage(AQQ, WM_SETTEXT, NULL, (LPARAM)Password.c_str());
        AQQ = FindWindow("TfrmLogon",NULL);
        AQQ = FindWindowEx(AQQ,NULL,"TButton","OK");
        if(AQQ!=NULL)
        {
          SendMessage(AQQ, BM_CLICK, 0, 0);
          Timer->Enabled=false;
          Close();
        }
      }
    //}
  }

  else
  {
    Count++;
    if(Count==200)
     Close();
  }
}
//---------------------------------------------------------------------------


