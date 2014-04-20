//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
#include <process.h>
#include <inifiles.hpp>
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//Utworzenie obiektow do struktur
TPluginAction PluginActionMenu;
TPluginAction PluginActionMakra;
TPluginLink PluginLink;
TPluginInfo PluginInfo;

int plugin_icon_idx; //Zmienna do ikony
bool Polish; //Do lokalizacji
UnicodeString Path;

//Zapisywanie zasobów
bool SaveResourceToFile(char *FileName, char *res)
{
  HRSRC hrsrc = FindResource(HInstance, res, RT_RCDATA);
  if(hrsrc == NULL) return false;
  DWORD size = SizeofResource(HInstance, hrsrc);
  HGLOBAL hglob = LoadResource(HInstance, hrsrc);
  LPVOID rdata = LockResource(hglob);
  HANDLE hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD writ;
  WriteFile(hFile, rdata, size, &writ, NULL);
  CloseHandle(hFile);
  return true;
}
//---------------------------------------------------------------------------

int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  //Katalog kompozycji
  Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,(WPARAM)(HInstance),0));
  Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(FileExists(Path + "\\\\Icons\\\\AQQRestarter.png"))
  {
	//Przypisanie ikony
	PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)((Path + "\\\\Icons\\\\AQQRestarter.png").w_str()));
  }
  else
  {
	//Katalog prywatny wtyczel
	Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
    Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

    if(!FileExists(Path + "\\\\AQQRestarter.png"))
    {
	  //Wypakowanie ikon
	  SaveResourceToFile((Path + "\\\\AQQRestarter.png").c_str(),"ID_PNG");
	  //Przypisanie ikony
	  PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)((Path + "\\\\AQQRestarter.png").w_str()));
      //Usuniecie ikony
      DeleteFile(Path + "\\\\AQQRestarter.png");
    }
    else
    {
	  //Przypisanie ikony
	  PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)((Path + "\\\\AQQRestarter.png").w_str()));
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Serwis restartu
int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  //Odczyt sciezki prywatnego profilu wtyczek
  Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  //Zapis sciezki AQQ, has³a oraz nazwy profilu do pliku
  TIniFile *Ini = new TIniFile(Path + "\\\\AQQRestarter.ini");
  //Odczyt has³a profilu
  UnicodeString Password = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  int x = AnsiPos("ProfilePass=",Password);
  Password.Delete(1,x+11);
  x = AnsiPos("\n",Password);
  Password.Delete(x,Password.Length());
  Ini->WriteString("Restarter", "Password", Password);
  //Odczyt PID procesu AQQ
  Ini->WriteString("Restarter", "PID", getpid());
  delete Ini;

  //Wypakowanie programu do restartowania AQQ
  Path = Path + "\\\\Restarter.exe";
  SaveResourceToFile(Path.c_str(),"ID_EXE");
  //Uruchomienie go
  ShellExecute(NULL, "open", Path.c_str(), NULL, NULL, SW_NORMAL);
  //Zamkniêcie AQQ
  PluginLink.CallService(AQQ_SYSTEM_RESTART,0,0);

  return 0;
}
//---------------------------------------------------------------------------

//Program
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  //Sprawdzanie wersji AQQ
  if (CompareVersion(AQQVersion,PLUGIN_MAKE_VERSION(2,1,0,55))<0)
  {
	MessageBox(Application->Handle,
	  "Wymagana wesja AQQ przez wtyczkê to minimum 2.1.0.55!\n"
	  "Wtyczka AQQ Restarter nie bêdzie dzia³aæ poprawnie!",
	  "Nieprawid³owa wersja AQQ",
	  MB_OK | MB_ICONEXCLAMATION);
  }
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"AQQ Restarter";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,0,1,0);
  PluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"sirbeherit@gmail.com";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

void PrzypiszSkrotMenu()
{
  PluginActionMenu.cbSize = sizeof(TPluginAction);
  PluginActionMenu.pszName = (wchar_t*)L"AQQRestarterPrzypiszSkrotMenu";
  if(Polish==1)
   PluginActionMenu.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  else
   PluginActionMenu.pszCaption = (wchar_t*) L"Restart AQQ";
  PluginActionMenu.Position = 12;
  PluginActionMenu.IconIndex = plugin_icon_idx;
  PluginActionMenu.pszService = (wchar_t*) L"serwis_aqqrestart";
  PluginActionMenu.pszPopupName = (wchar_t*) L"muProgram";

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionMenu));
}
//---------------------------------------------------------------------------

void PrzypiszSkrotMakra()
{
  PluginActionMakra.cbSize = sizeof(TPluginAction);
  PluginActionMakra.pszName = (wchar_t*)L"AQQRestarterPrzypiszSkrotMakra";
  if(Polish==1)
   PluginActionMakra.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  else
   PluginActionMakra.pszCaption = (wchar_t*) L"Restart AQQ";
  PluginActionMakra.Position = 14;
  PluginActionMakra.IconIndex = plugin_icon_idx;
  PluginActionMakra.pszService = (wchar_t*) L"serwis_aqqrestart";
  PluginActionMakra.pszPopupName = (wchar_t*) L"popMacros";

  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionMakra));
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  //Rozpoznanie lokalizacji
  UnicodeString Lang = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETLANGSTR,0,(LPARAM)(L"Password")));
  if(Lang=="Has³o")
   Polish=1;
  else
   Polish=0;
  //Koniec

  //Katalog aktywnej kompozycji
  Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,(WPARAM)(HInstance),0));
  Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(FileExists(Path + "\\\\Icons\\\\AQQRestarter.png"))
  {
	//Przypisanie ikony
	plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)((Path + "\\\\Icons\\\\AQQRestarter.png").w_str()));
  }
  else
  {
    //Katalog prywatny wtyczel
	Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
	Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

	if(!FileExists(Path + "\\\\AQQRestarter.png"))
	{
	  //Wypakowanie ikon
	  SaveResourceToFile((Path + "\\\\AQQRestarter.png").c_str(),"ID_PNG");
	  //Przypisanie ikony
	  plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)((Path + "\\\\AQQRestarter.png").w_str()));
      //Usuniecie ikony
      DeleteFile(Path + "\\\\AQQRestarter.png");
    }
    else
    {
	  //Przypisanie ikony
	  plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)((Path + "\\\\AQQRestarter.png").w_str()));
    }
  }

  Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  if(FileExists(Path + "\\\\Restarter.exe"))
   DeleteFile(Path + "\\\\Restarter.exe");

  //Utworzenie serwisu restartu
  PluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
  //Przypisanie skrotow
  PrzypiszSkrotMenu();
  PrzypiszSkrotMakra();

  //Hook na zmianê kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED, OnThemeChanged);

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  PluginLink.UnhookEvent(OnThemeChanged);
  PluginLink.DestroyServiceFunction(AqqReStartService);
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionMenu));
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionMakra));

  return 0;
}
//---------------------------------------------------------------------------

