#pragma hdrstop
#pragma argsused
#include <tchar.h>
#include <windows.h>
//---------------------------------------------------------------------------

//Interwal wylaczenie programu
int GiveUpCount = 0;

int _tmain(int argc, _TCHAR* argv[])
{
  //Hook dla petli
  START:
  //Ukrywanie chmurki informacyjnej
  HWND hWindowHandle = FindWindow(L"TfrmMiniStatus",NULL);
  if(hWindowHandle) ShowWindow(hWindowHandle, SW_HIDE);
  //Szukanie okna logowania
  hWindowHandle = FindWindow(L"TfrmLogon",NULL);
  //Jezeli okno zostalo znalezione
  if((hWindowHandle)&&(IsWindow(hWindowHandle))&&(IsWindowVisible(hWindowHandle)))
  {
	//Szukanie kontrolki przycisku OK
	hWindowHandle = FindWindowEx(hWindowHandle,NULL,L"TsButton",L"OK");
	//Kontrolka zostala znaleziona
	if(hWindowHandle)
	{
	  //Wysylanie emulacji klikniecia
	  SendMessage(hWindowHandle, BM_CLICK, 0, 0);
	  //Zamkniecie programu
	  return 0;
	}
	//Kontrolka nie zostala znaleziona
	else
	{
	  //Interwal wylaczenie programu
	  GiveUpCount++;
	  //Interwal wylaczenia programu zostal osiagniety
	  if(GiveUpCount==200)
	   return 0;
	  //Uspienie programu na 100ms
	  else
	  {
		Sleep(100);
		goto START;
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
	 return 0;
	//Uspienie programu na 100ms
	else
	{
	  Sleep(100);
	  goto START;
	}
  }

  return 0;
}
//---------------------------------------------------------------------------
