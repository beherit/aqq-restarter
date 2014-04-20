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
int Count = 0;
HWND hWindowHandle;
DWORD PID;
//---------------------------------------------------------------------------

//Pobieranie uchwytu okna przez PID
HWND HwndPID(DWORD dwPID)
{
  HWND Hwnd = GetTopWindow(0);
  HWND hWnd = 0;
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

//Przechodzenie przez okno logowania
void __fastcall TRestarterForm::TimerTimer(TObject *Sender)
{
  hWindowHandle = FindWindow("TfrmLogon",NULL);
  if(hWindowHandle)
  {
	hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TEdit",NULL);
	if(!hWindowHandle)
	{
	  hWindowHandle = FindWindow("TfrmLogon",NULL);
	  hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TsEdit",NULL);
	}
	if(hWindowHandle)
	{
	  SendMessage(hWindowHandle, WM_SETTEXT, NULL, (LPARAM)Password.c_str());
	  hWindowHandle = FindWindow("TfrmLogon",NULL);
	  hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TButton","OK");
	  if(!hWindowHandle)
	  {
        hWindowHandle = FindWindow("TfrmLogon",NULL);
		hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TsButton","OK");
      }
	  if(hWindowHandle)
	  {
		SendMessage(hWindowHandle, BM_CLICK, 0, 0);
		Timer->Enabled = false;
		Close();
	  }
	  else
	  {
		Count++;
		if(Count==200)
		 Close();
	  }
	}
	else
	{
	  Count++;
	  if(Count==200)
	   Close();
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

//Pobieranie danych z pliku INI utworzonego przez wtyczke
void __fastcall TRestarterForm::FormShow(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");
  Password = Ini->ReadString("Restarter", "Password", "");
  PID = Ini->ReadInteger("Restarter", "PID", 0);
  delete Ini;

  //Usuwanie pliku INI
  if(FileExists(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini"))
   DeleteFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");

  if(PID)
  {
	Password=IdDecoderMIME->DecodeString(Password);
	UTF8String PasswordUTF8 = Password;
	Password=Utf8ToAnsi(PasswordUTF8);
	ProcessTimer->Enabled = true;
  }
  else
   Close();
}
//---------------------------------------------------------------------------

//Czekanie na zamkniecie siê poprzedniej instancji AQQ
void __fastcall TRestarterForm::ProcessTimerTimer(TObject *Sender)
{
  if(!HwndPID(PID))
  {
	ProcessTimer->Enabled = false;
	Timer->Enabled = true;
  }
}
//---------------------------------------------------------------------------

//Ukrywanie formy programu
void __fastcall TRestarterForm::FormPaint(TObject *Sender)
{
  ShowWindow(Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

//Ukrywanie okna logowania i chmurki informacyjnej
void __fastcall TRestarterForm::HideTimerTimer(TObject *Sender)
{
  hWindowHandle = FindWindow("TfrmLogon",NULL);
  if(hWindowHandle) ShowWindow(hWindowHandle, SW_HIDE);

  hWindowHandle = FindWindow("TfrmMiniStatus",NULL);
  if(hWindowHandle)	ShowWindow(hWindowHandle, SW_HIDE);
}
//---------------------------------------------------------------------------

