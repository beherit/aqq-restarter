//#include <vcl.h>
//#include <windows.h>
#include <System.hpp>
#include <PluginAPI.h>
#pragma hdrstop
#pragma argsused
#include <inifiles.hpp>
#define AQQRESTARTER_SYSTEM_RESTART L"AQQRestarter/System/Restart"
#define AQQRESTARTER_SYSTEM_RESTARTING L"AQQRestarter/System/Restarting"

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
UnicodeString GetPluginUserDirW()
{
  return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
  TPluginTwoFlagParams PluginTwoFlagParams;
  PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
  PluginTwoFlagParams.Param1 = ResName;
  PluginTwoFlagParams.Param2 = ResType;
  PluginTwoFlagParams.Flag1 = (int)HInstance;
  PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy profil jest zabezpieczony haslem
bool ChkPassEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile("ChkPassEnabled.ini");
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString SkinsEnabled = Settings->ReadString("Security","ProfilePassActive","0");
  delete Settings;
  return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest opcja logowania
bool ChkAutologonEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile("ChkAutologonEnabled");
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString SkinsEnabled = Settings->ReadString("Settings","AutoLogin","0");
  delete Settings;
  return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Serwis restartu
int __stdcall SystemRestart(WPARAM wParam, LPARAM lParam)
{
  //Wysy³anie notyfikacji AQQRESTARTER_SYSTEM_RESTARTING
  TPluginHook PluginHook;
  PluginHook.HookName = AQQRESTARTER_SYSTEM_RESTARTING;
  PluginHook.wParam = 0;
  PluginHook.lParam = 0;
  PluginLink.CallService(AQQ_SYSTEM_SENDHOOK,(WPARAM)(&PluginHook),0);
  //Profil niezabezpiecony haslem bez opcji autologowania
  if((!ChkPassEnabled())&&(!ChkAutologonEnabled()))
  {
	//Wypakowanie programu do wspomgania restartu AQQ
	ExtractRes((GetPluginUserDirW() + "\\Restarter.exe").w_str(),L"RESTARTER",L"DATA");
	//Uruchomienie programu do wspomgania restartu AQQ
	ShellExecute(NULL, L"open", (GetPluginUserDir()+"\\\\Restarter.exe").w_str(), NULL, NULL, SW_HIDE);
  }
  //Restart AQQ z dzierzawa uprawnien
  PluginLink.CallService(AQQ_SYSTEM_RESTART,1,0);
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
	  ExtractRes((GetPluginUserDirW()+"\\AQQRestarter.png").w_str(),L"ICON",L"DATA");
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
  PluginActionProgramItem.pszCaption = L"Zrestartuj AQQ";
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
  PluginActionMacrosItem.pszCaption = L"Zrestartuj AQQ";
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
	  ExtractRes((GetPluginUserDirW()+"\\AQQRestarter.png").w_str(),L"ICON",L"DATA");
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,3,0,0);
  PluginInfo.Description = L"Szybki restart komunikatora z pozycji menu Program, menu kompaktowego lub menu makr z zasobnika systemowego.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
