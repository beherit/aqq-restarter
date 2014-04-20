//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop    
#pragma argsused
#include "Aqq.h"
#include <memory> 
#include "Files.rh"
//#include <process.h>
#include <inifiles.hpp>
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}
//---------------------------------------------------------------------------

//zamiana AnsiString->wchar_t*
wchar_t* AnsiTowchar_t(AnsiString Str)
{                                 
  const char* Text = Str.c_str();
  int size = MultiByteToWideChar(GetACP(), 0, Text, -1, 0,0);
  wchar_t* wbuffer = new wchar_t[size+1];

  MultiByteToWideChar(GetACP(), 0, Text, -1, wbuffer, size+1);

  return wbuffer;
}
//---------------------------------------------------------------------------

bool Polish; //Do lokalizacji

//Utworzenie obiektow do struktur
TPluginAction PluginActionMenu;
TPluginAction PluginActionMakra;
TPluginLink PluginLink;
TPluginInfo PluginInfo;

int plugin_icon_idx; //Zmienna do ikony

//Do wypakowywania RES
void ExtractExe(unsigned short ID, AnsiString FileName)
{
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;

  TMemoryStream *Ms = new TMemoryStream;

  Ms->Position = 0;
  Ms->LoadFromStream(stream.get());
  Ms->Position = 0;
  Ms->SaveToFile(FileName);
  Ms->Free();
}
//---------------------------------------------------------------------------

int __stdcall OnThemeChanged (WPARAM wParam, LPARAM lParam)
{
  //Katalog kompozycji
  AnsiString Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,(WPARAM)(hInstance),0));
  Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(FileExists(Path + "\\\\Icons\\\\AQQRestarter.png"))
  {
    //Przypisanie ikony
    wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\Icons\\\\AQQRestarter.png");
    PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)(plugin_icon_path));
  }
  else
  {
    //Katalog prywatny wtyczel
    Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
    Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

    if(!FileExists(Path + "\\\\AQQRestarter.png"))
    {
      //Wypakowanie ikon
      ExtractExe(ID_PNG,Path + "\\\\AQQRestarter.png");
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\AQQRestarter.png");
      PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)(plugin_icon_path));
      //Usuniecie ikony
      DeleteFile(Path + "\\\\AQQRestarter.png");
    }
    else
    {
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\AQQRestarter.png");
      PluginLink.CallService(AQQ_ICONS_REPLACEPNGICON,(WPARAM)(plugin_icon_idx),(LPARAM)(plugin_icon_path));
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

//Serwis restartu
int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  //Odczyt sciezki prywatnego profilu wtyczek
  AnsiString PluginPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  //Zapis sciezki AQQ, has³a oraz nazwy profilu do pliku
  TIniFile *Ini = new TIniFile(PluginPath + "\\\\AQQRestarter.ini");
  Ini->WriteString("Restarter", "AQQPath", (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETAPPFILEPATH,0,0)));
  //Odczyt has³a profilu
  AnsiString Password = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  int x = AnsiPos("ProfilePass=",Password);
  Password.Delete(1,x+11);
  x = AnsiPos("\n",Password);
  Password.Delete(x,Password.Length());
  Ini->WriteString("Restarter", "Password", Password);
  //Odczyt nazwy profilu
  //AnsiString ProfileName = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(hInstance),0));
  //x = ProfileName.LastDelimiter("\\");
  //ProfileName.Delete(1,x);
  //ProfileName=ProfileName.Trim();
  //Ini->WriteString("Restarter", "Profile", ProfileName);
  delete Ini;

  //Wypakowanie programu do restartowania AQQ
  PluginPath = PluginPath + "\\\\Restarter.exe";
  ExtractExe(ID_EXE,PluginPath);
  //Uruchomienie go
  ShellExecute(NULL, "open", PluginPath.c_str(), NULL, NULL, SW_SHOWMINIMIZED);
  //Zamkniêcie AQQ
  PluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)(L"aExit"));

  return 0;
}
//---------------------------------------------------------------------------

//Program
extern "C"  __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t*)L"AQQ Restarter";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,4,2);
  PluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"";

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
  PluginActionMenu.Position = 11;
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
  AnsiString Lang = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETLANGSTR,0,(LPARAM)(L"Password")));
  if(Lang=="Has³o")
   Polish=1;
  else
   Polish=0;
  //Koniec

  //Katalog aktywnej kompozycji
  AnsiString Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,(WPARAM)(hInstance),0));
  Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(FileExists(Path + "\\\\Icons\\\\AQQRestarter.png"))
  {
    //Przypisanie ikony
    wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\Icons\\\\AQQRestarter.png");
    plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  }
  else
  {
    //Katalog prywatny wtyczel
    Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
    Path = StringReplace(Path, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

    if(!FileExists(Path + "\\\\AQQRestarter.png"))
    {
      //Wypakowanie ikon
      ExtractExe(ID_PNG,Path + "\\\\AQQRestarter.png");
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\AQQRestarter.png");
      plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
      //Usuniecie ikony
      DeleteFile(Path + "\\\\AQQRestarter.png");
    }
    else
    {
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(Path + "\\\\AQQRestarter.png");
      plugin_icon_idx=PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
    }
  }

  Path = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
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
