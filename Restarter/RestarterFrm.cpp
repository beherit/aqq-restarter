//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RestarterFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
__fastcall TRestarterForm::TRestarterForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
UnicodeString Password;
int Count=0;
HWND AQQ;
DWORD PID;
//---------------------------------------------------------------------------

//Pobieranie uchwytu okna przez PID
HWND HwndPID(DWORD dwPID)
{
  HWND Hwnd = GetTopWindow(0);
  HWND hWnd = 0;;
  DWORD pid;

  while(Hwnd)
  {
    GetWindowThreadProcessId(Hwnd, &pid);
    if(pid == dwPID) hWnd = Hwnd;
    Hwnd = GetNextWindow(Hwnd, GW_HWNDNEXT);
  }

  return hWnd;
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::TimerTimer(TObject *Sender)
{
  AQQ = FindWindow("TfrmLogon",NULL);
  if(AQQ!=NULL)
  {
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
  }
  else
  {
    Count++;
    if(Count==200)
     Close();
  }
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::FormShow(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");
  Password = Ini->ReadString("Restarter", "Password", "");
  PID = Ini->ReadInteger("Restarter", "PID", 0);
  delete Ini;

  //Usuwanie pliku INI
  if(FileExists(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini"))
   DeleteFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");

  if(PID!=0)
  {
	Password=IdDecoderMIME->DecodeString(Password);
	UTF8String PasswordUTF8 = Password;
	Password=Utf8ToAnsi(PasswordUTF8);
	ProcessTimer->Enabled=true;
  }
  else
   Close();
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::ProcessTimerTimer(TObject *Sender)
{
  if(HwndPID(PID)==NULL)
  {
	ProcessTimer->Enabled=false;
	Timer->Enabled=true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::FormPaint(TObject *Sender)
{
  ShowWindow(Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::HideTimerTimer(TObject *Sender)
{
  AQQ = FindWindow("TfrmLogon",NULL);
  if(AQQ!=NULL)
  {
	ShowWindow(AQQ, SW_HIDE);
  }
  AQQ = FindWindow("TfrmMiniStatus",NULL);
  if(AQQ!=NULL)
  {
	ShowWindow(AQQ, SW_HIDE);
  }
}
//---------------------------------------------------------------------------

