//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RestarterFrm.h"
#include <inifiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdCoder"
#pragma link "IdCoder3to4"
#pragma link "IdCoderMIME"
#pragma resource "*.dfm"
TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
//Haslo do profilu AQQ
UnicodeString Password;
//Interwal wylaczenie programu
int GiveUpCount = 0;
//PID poprzedniej instancji AQQ
DWORD PID;
//---------------------------------------------------------------------------

__fastcall TRestarterForm::TRestarterForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

//Pobieranie uchwytu okna przez PID
HWND HwndFromPID(DWORD dwPID)
{
  //Pobieranie uchwytu do okna na wierzchu
  HWND TopHwnd = GetTopWindow(0);
  //Uchwyt pomocniczy
  HWND hWnd = 0;
  //PID procesu
  DWORD PID;
  //Szukanie okna po jego PID
  while(TopHwnd)
  {
	//Pobieranie PID okna
	GetWindowThreadProcessId(TopHwnd, &PID);
	//Jezeli PID okna jest taki sam
	if(PID==dwPID)
	{
	  //Zapisanie uchwytu do okna
	  hWnd = TopHwnd;
	  //Zatrzymanie petli
	  TopHwnd = 0;
	}
	//Szukanie kolejnego okna
	TopHwnd = GetNextWindow(TopHwnd, GW_HWNDNEXT);
  }
  //Przekazanie uchwytu do okna
  return hWnd;
}
//---------------------------------------------------------------------------

//Ukrywanie formy programu
void __fastcall TRestarterForm::FormPaint(TObject *Sender)
{
  ShowWindow(Handle, SW_HIDE);
}
//---------------------------------------------------------------------------

//Pobieranie danych z pliku INI utworzonego przez wtyczke
void __fastcall TRestarterForm::FormShow(TObject *Sender)
{
  //Odczyt danych poprzedniej instacji AQQ z pliku INI
  TIniFile *Ini = new TIniFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");
  //Odczyt hasla
  Password = Ini->ReadString("Restarter", "Password", "");
  //Odczyt PID procesu poprzedniej instancji AQQ
  PID = Ini->ReadInteger("Restarter", "PID", 0);
  delete Ini;
  //Usuwanie pliku INI
  if(FileExists(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini"))
   DeleteFile(ExtractFilePath(Application->ExeName) + "\\\\AQQRestarter.ini");
  //Jezeli udalo sie pobrac PID procesu
  if(PID)
  {
	//Dekodowanie hasla
	Password=IdDecoderMIME->DecodeString(Password);
	UTF8String PasswordUTF8 = Password;
	Password = Utf8ToAnsi(PasswordUTF8);
	//Wlaczenie timera szukania poprzedniej instancji AQQ
	ProcessTimer->Enabled = true;
  }
  //Nie udalo sie pobrac PID procesu
  else
   Close();
}
//---------------------------------------------------------------------------

//Czekanie na zamkniecie siê poprzedniej instancji AQQ
void __fastcall TRestarterForm::ProcessTimerTimer(TObject *Sender)
{
  //Gdy poprzednia instancja AQQ juz nie istnieje
  if(!HwndFromPID(PID))
  {
	//Wylaczenie timera
	ProcessTimer->Enabled = false;
	//Wlaczenie timera przechodzenia przez okno logowania
	Timer->Enabled = true;
  }
}
//---------------------------------------------------------------------------

//Przechodzenie przez okno logowania
void __fastcall TRestarterForm::TimerTimer(TObject *Sender)
{
  //Zatrzymanie timera
  Timer->Enabled = false;
  //Szukanie okna logowania
  HWND hWindowHandle = FindWindow("TfrmLogon",NULL);
  //Jezeli okno zostalo znalezione
  if((hWindowHandle)&&(IsWindow(hWindowHandle))&&(IsWindowVisible(hWindowHandle)))
  {
	//Jezeli trzeba wprowadzic haslo
	if(!Password.IsEmpty())
	{
	  //Szukanie kontrolki do wpisywania hasla
	  hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TsEdit",NULL);
	  //Kontrolka zostala znaleziona
	  if(hWindowHandle)
	  {
		//Umieszanie hasla w kontrolce
		SendMessageW(hWindowHandle, WM_SETTEXT, NULL, (LPARAM)Password.w_str());
		//Szukanie kontrolki przycisku OK
		hWindowHandle = FindWindow("TfrmLogon",NULL);
		hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TsButton","OK");
		//Kontrolka zostala znaleziona
		if(hWindowHandle)
		{
		  //Wylaczenie timera
		  Timer->Enabled = false;
		  //Wysylanie emulacji klikniecia
		  SendMessage(hWindowHandle, BM_CLICK, 0, 0);
		  //Zamkniecie programu
		  Close();
		}
		//Kontrolka nie zostala znaleziona
		else
		{
		  //Interwal wylaczenie programu
		  GiveUpCount++;
		  //Interwal wylaczenia programu zostal osiagniety
		  if(GiveUpCount==200)
		   Close();
		  //Wlaczenie timera
		  else
		   Timer->Enabled = true;
		}
	  }
	  //Kontrolka nie zostala znaleziona
	  else
	  {
		//Interwal wylaczenie programu
		GiveUpCount++;
		//Interwal wylaczenia programu zostal osiagniety
		if(GiveUpCount==200)
		 Close();
		//Wlaczenie timera
		else
		 Timer->Enabled = true;
	  }
	}
	//Wpisywanie hasla jest zbedne
	else
	{
      //Szukanie kontrolki przycisku OK
	  hWindowHandle = FindWindow("TfrmLogon",NULL);
	  hWindowHandle = FindWindowEx(hWindowHandle,NULL,"TsButton","OK");
	  //Kontrolka zostala znaleziona
	  if(hWindowHandle)
	  {
		//Wylaczenie timera
		Timer->Enabled = false;
		//Wysylanie emulacji klikniecia
		SendMessage(hWindowHandle, BM_CLICK, 0, 0);
		//Zamkniecie programu
		Close();
	  }
	  //Kontrolka nie zostala znaleziona
	  else
	  {
		//Interwal wylaczenie programu
		GiveUpCount++;
		//Interwal wylaczenia programu zostal osiagniety
		if(GiveUpCount==200)
		 Close();
		//Wlaczenie timera
		else
		 Timer->Enabled = true;
	  }
    }
  }
  //Okno nie zostalo znalezione
  else
  {
	//Interwal wylaczenie programu
	GiveUpCount++;
	//Interwal wylaczenia programu zostal osiagniety
	if(GiveUpCount==200)
	 Close();
	//Wlaczenie timera
	else
	 Timer->Enabled = true;
  }
}
//---------------------------------------------------------------------------

//Ukrywanie okna logowania i chmurki informacyjnej
void __fastcall TRestarterForm::HideTimerTimer(TObject *Sender)
{
  //Oko logowania
  //HWND hFrmLogon = FindWindow("TfrmLogon",NULL);
  //if(hFrmLogon) ShowWindow(hFrmLogon, SW_HIDE);
  //Chmurka informacyjna
  HWND hFrmMiniStatus = FindWindow("TfrmMiniStatus",NULL);
  if(hFrmMiniStatus) ShowWindow(hFrmMiniStatus, SW_HIDE);
}
//---------------------------------------------------------------------------

