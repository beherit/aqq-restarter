#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <PluginAPI.h>
#include <IdHashMessageDigest.hpp>
#define AQQRESTARTER_SYSTEM_RESTART L"AQQRestarter/System/Restart"

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

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
  if(FileExists(FileName))
  {
	UnicodeString Result;
	TFileStream *fs;

	fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
	try
	{
	  TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
	  try
	  {
		Result = idmd5->HashStreamAsHex(fs);
	  }
	  __finally
	  {
		delete idmd5;
	  }
	}
	__finally
	{
	  delete fs;
	}

	return Result;
  }
  else
   return 0;
}
//---------------------------------------------------------------------------

//Serwis restartu
int __stdcall SystemRestart(WPARAM wParam, LPARAM lParam)
{
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
	AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(ThemeDir+"\\\\Icons\\\\AQQRestarter.png").w_str());
  }
  //Jezeli kompozycja nie posiada wlasnej ikonki dla wtyczki
  else
  {
	//Aktualizacja ikony w interfejsie (z usuwaniem)
	if(!FileExists(GetPluginUserDir()+"\\\\AQQRestarter.png"))
	{
	  //Wypakowanie ikony
	  ExtractRes((GetPluginUserDir()+"\\\\AQQRestarter.png").w_str(),L"FASTACCESS",L"DATA");
	  //Aktualizacja ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(GetPluginUserDir()+"\\\\AQQRestarter.png").w_str());
	  //Usuniecie ikony
	  DeleteFile(GetPluginUserDir()+"\\\\AQQRestarter.png");
	}
	//Aktualizacja ikony w interfejsie (bez usuwania)
	else
	{
	  //Aktualizacja ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,AQQRESTARTER,(LPARAM)(GetPluginUserDir()+"\\\\AQQRestarter.png").w_str());
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
  //Jezeli kompozycja posiada wlasna ikonke dla wtyczki
  if(FileExists(GetThemeDir()+"\\\\Icons\\\\AQQRestarter.png"))
  {
	//Przypisanie ikony w interfejsie
	AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(GetThemeDir()+"\\\\Icons\\\\AQQRestarter.png").w_str());
  }
  else
  {
	//Aktualizacja ikony w interfejsie (z usuwaniem)
	if(!FileExists(GetPluginUserDir()+"\\\\AQQRestarter.png"))
	{
	  //Wypakowanie ikony
	  ExtractRes((GetPluginUserDir()+"\\AQQRestarter.png").w_str(),L"FASTACCESS",L"DATA");
	  //Przypisanie ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(GetPluginUserDir()+"\\\\AQQRestarter.png").w_str());
	  //Usuniecie ikony
	  DeleteFile(GetPluginUserDir()+"\\\\AQQRestarter.png");
	}
	//Aktualizacja ikony w interfejsie (bez usuwania)
	else
	{
	  //Przypisanie ikony w interfejsie
	  AQQRESTARTER = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0,(LPARAM)(GetPluginUserDir()+"\\\\AQQRestarter.png").w_str());
	}
  }
  //Wypakiwanie ikonki AQQRestarter.dll.png
  //9C5C93C35D61AF3149D8382A07E131FB
  if(!DirectoryExists(GetPluginUserDir()+"\\\\Shared"))
   CreateDir(GetPluginUserDir()+"\\\\Shared");
  if(!FileExists(GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png"))
   ExtractRes((GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
  else if(MD5File(GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png")!="992F252A9087DBD036BA98B6D5DE2A08")
   ExtractRes((GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
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
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"AQQ Restarter";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,3,1,0);
  PluginInfo.Description = L"Szybki restart komunikatora z pozycji menu Program, menu kompaktowego lub menu makr z zasobnika systemowego.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------
