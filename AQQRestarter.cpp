//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
#include <process.h>
#include <inifiles.hpp>
#define AQQRESTARTER_SYSTEM_RESTART L"AQQRestarter/System/Restart"
#define AQQRESTARTER_SYSTEM_RESTARTING L"AQQRestarter/System/Restarting"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//Struktury-elementow-w-menu-------------------------------------------------
TPluginAction PluginActionProgramItem;
TPluginAction PluginActionMacrosItem;
//IKONA-W-INTERFEJSIE--------------------------------------------------------
int AQQRESTARTER;
//LOKALIZACJA----------------------------------------------------------------
bool Polish;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
int __stdcall SystemRestart(WPARAM wParam, LPARAM lParam);
int __stdcall OnThemeChanged(WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

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

//Serwis restartu
int __stdcall SystemRestart(WPARAM, LPARAM)
{
  //Wysy³anie notyfikacji AQQRESTARTER_SYSTEM_RESTARTING
  TPluginHook PluginHook;
  PluginHook.HookName = AQQRESTARTER_SYSTEM_RESTARTING;
  PluginHook.wParam = 0;
  PluginHook.lParam = 0;
  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
  //Pobieranie sciezki katalogu prywatnego wtyczek
  UnicodeString PluginUserDir = GetPluginUserDir();
  //Zapis sciezki AQQ, has³a oraz nazwy profilu do pliku
  TIniFile *Ini = new TIniFile(PluginUserDir+"\\\\AQQRestarter.ini");
  //Odczyt has³a profilu
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  if(StrToBool(Settings->ReadString("Security","ProfilePassActive","1")))
  {
	UnicodeString Password = Settings->ReadString("Security","ProfilePass","");
	Ini->WriteString("Restarter", "Password", Password);
  }
  delete Settings;
  //Odczyt PID procesu AQQ
  Ini->WriteString("Restarter", "PID", getpid());
  delete Ini;
  //Wypakowanie programu do wspomgania restartu AQQ
  PluginUserDir = PluginUserDir + "\\\\Restarter.exe";
  SaveResourceToFile(PluginUserDir.t_str(),"ID_EXE");
  //Uruchomienie programu do wspomgania restartu AQQ
  ShellExecute(NULL, "open", PluginUserDir.t_str(), NULL, NULL, SW_NORMAL);
  //Zamkniêcie AQQ
  PluginLink.CallService(AQQ_SYSTEM_RESTART,0,0);
  //Zwrot w funkcji
  return 1;
}
//---------------------------------------------------------------------------

//Hook na zmianê kompozycji
int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  //Pobieranie sciezki nowej aktywnej kompozycji
  UnicodeString ThemeDir = StringReplace((wchar_t*)lParam, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  //Jezeli kompozycja posiada wlasna ikonke dla wtyczki
  if(FileExists(ThemeDir+"\\\\Icons\\\\AQQRestarter.png"))
  {
	//Aktualizacja ikony w interfejsie
	AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(ThemeDir + "\\\\Icons\\\\AQQRestarter.png").w_str());
  }
  //Jezeli kompozycja nie posiada wlasnej ikonki dla wtyczki
  else
  {
	//Pobieranie sciezki katalogu prywatnego wtyczek
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Aktualizacja ikony w interfejsie (z usuwaniem)
	if(!FileExists(PluginUserDir+"\\\\AQQRestarter.png"))
	{
	  //Wypakowanie ikony
	  SaveResourceToFile((PluginUserDir+"\\\\AQQRestarter.png").t_str(),"ID_PNG");
	  //Aktualizacja ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(PluginUserDir+"\\\\AQQRestarter.png").w_str());
	  //Usuniecie ikony
	  DeleteFile(PluginUserDir+"\\\\AQQRestarter.png");
	}
	//Aktualizacja ikony w interfejsie (bez usuwania)
	else
	{
	  //Aktualizacja ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(PluginUserDir+"\\\\AQQRestarter.png").w_str());
	}
  }

  return 0;
}
//---------------------------------------------------------------------------

//Tworzenie elementu w menu Program
void BuildProgramItem()
{
  //Ustalanie pozycji elementu "Zakoncz"
  TPluginItemDescriber PluginItemDescriber;
  PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
  PluginItemDescriber.ParentName = (wchar_t*)L"muProgram";
  PluginItemDescriber.Name = (wchar_t*)L"Zakocz2";
  PPluginAction Action = (PPluginAction)(PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber)));
  //Wypelnianie struktury
  PluginActionProgramItem.cbSize = sizeof(TPluginAction);
  PluginActionProgramItem.pszName = L"AQQRestarterProgramItem";
  if(Polish)
   PluginActionProgramItem.pszCaption = L"Zrestartuj AQQ";
  else
   PluginActionProgramItem.pszCaption = L"Restart AQQ";
  PluginActionProgramItem.Position = Action->Position - 2;
  PluginActionProgramItem.IconIndex = AQQRESTARTER;
  PluginActionProgramItem.pszService = L"sAQQRestarterSystemRestarte";
  PluginActionProgramItem.pszPopupName = L"muProgram";
  //Tworzenie elementu w interfejsie
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionProgramItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu w menu makr
void BuildMacrosItem()
{
  //Wypelnianie struktury
  PluginActionMacrosItem.cbSize = sizeof(TPluginAction);
  PluginActionMacrosItem.pszName = L"AQQRestarterMacrosIte";
  if(Polish)
   PluginActionMacrosItem.pszCaption = L"Zrestartuj AQQ";
  else
   PluginActionMacrosItem.pszCaption = L"Restart AQQ";
  PluginActionMacrosItem.Position = 14;
  PluginActionMacrosItem.IconIndex = AQQRESTARTER;
  PluginActionMacrosItem.pszService = L"sAQQRestarterSystemRestarte";
  PluginActionMacrosItem.pszPopupName = L"popMacros";
  //Tworzenie elementu w interfejsie
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionMacrosItem));
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Rozpoznanie lokalizacji
  UnicodeString Lang = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETLANGSTR,0,(LPARAM)(L"Password")));
  if(Lang=="Has³o")
   Polish = true;
  else
   Polish = false;
  //Pobieranie sciezki do skorki kompozycji
  UnicodeString Path = GetThemeDir();
  //Jezeli kompozycja posiada wlasna ikonke dla wtyczki
  if(FileExists(Path+"\\\\Icons\\\\AQQRestarter.png"))
  {
    //Przypisanie ikony w interfejsie
	AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(Path+"\\\\Icons\\\\AQQRestarter.png").w_str());
  }
  else
  {
	//Pobieranie sciezki katalogu prywatnego wtyczek
	Path = GetPluginUserDir();
	//Aktualizacja ikony w interfejsie (z usuwaniem)
	if(!FileExists(Path+"\\\\AQQRestarter.png"))
	{
	  //Wypakowanie ikony
	  SaveResourceToFile((Path+"\\\\AQQRestarter.png").t_str(),"ID_PNG");
	  //Przypisanie ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(Path+"\\\\AQQRestarter.png").w_str());
	  //Usuniecie ikony
	  DeleteFile(Path+"\\\\AQQRestarter.png");
	}
	//Aktualizacja ikony w interfejsie (bez usuwania)
	else
	{
	  //Przypisanie ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(Path+"\\\\AQQRestarter.png").w_str());
	}
  }
  //Pobieranie sciezki katalogu prywatnego wtyczek
  Path = GetPluginUserDir();
  //Usuwanie programu do wspomgania restartu AQQ
  if(FileExists(Path + "\\\\Restarter.exe"))
   DeleteFile(Path + "\\\\Restarter.exe");
  //Tworzenie serwisu restartu
  PluginLink.CreateServiceFunction(L"sAQQRestarterSystemRestarte",SystemRestart);
  //Tworzenie elementow w interfejsie
  BuildProgramItem();
  BuildMacrosItem();
  //Hook SDK wtyczki
  PluginLink.HookEvent(AQQRESTARTER_SYSTEM_RESTART,SystemRestart);
  //Hook na zmianê kompozycji
  PluginLink.HookEvent(AQQ_SYSTEM_THEMECHANGED,OnThemeChanged);

  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(SystemRestart);
  PluginLink.UnhookEvent(OnThemeChanged);
  //Usuwanie serwisu restartu
  PluginLink.DestroyServiceFunction(SystemRestart);
  //Usuwanie elementow w interfejsie
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionProgramItem));
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionMacrosItem));
  //Usuwanie ikony w interfejsie
  PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)AQQRESTARTER);

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"AQQ Restarter";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,1,3,0);
  PluginInfo.Description = L"Szybki restart komunikatora z pozycji menu Program, menu kompaktowego lub menu makr z zasobnika systemowego.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

