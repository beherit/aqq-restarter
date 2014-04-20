//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop    
#pragma argsused
#include "Aqq.h"  
#include <memory> 
#include "ikonka.rh"
#include <process.h>
#include <inifiles.hpp>
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}
//---------------------------------------------------------------------------

wchar_t *AnsiTowchar_t(AnsiString Str) //zamiana AnsiString->wchar_t*
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
}
//---------------------------------------------------------------------------

//Utworzenie obiektow do struktur
PluginAction TPluginActionMenu;
PluginAction TPluginActionMakra;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

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

//Serwis restartu
int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  //Odczyt sciezki prywatnego profilu wtyczek
  AnsiString PluginPath = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  //Odczyt PID AQQ
  int PID = getpid();
  //Zapis PID do pliku
  TIniFile *Ini = new TIniFile(PluginPath + "\\\\Restarter.ini");
  Ini->WriteInteger("Restarter", "AQQPid", PID);
  delete Ini;
  //Wypakowanie programu do restartowania AQQ
  PluginPath = PluginPath + "\\\\Restarter.exe";
  ExtractExe(ID_EXE,PluginPath);
  //Uruchomienie go
  WinExec(PluginPath.c_str(), SW_HIDE);

  return 0;
}
//---------------------------------------------------------------------------

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"AQQ Restarter";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,2,0);
  TPluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"";

  return &TPluginInfo;
}
//---------------------------------------------------------------------------

void PrzypiszSkrotMenu()
{
  TPluginActionMenu.cbSize = sizeof(PluginAction);
  TPluginActionMenu.pszName = (wchar_t*)L"AQQRestarterPrzypiszSkrotMenu";
  TPluginActionMenu.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginActionMenu.Position = 12;
  TPluginActionMenu.IconIndex = plugin_icon_idx;
  TPluginActionMenu.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginActionMenu.pszPopupName = (wchar_t*) L"muProgram";

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMenu));
}
//---------------------------------------------------------------------------

void PrzypiszSkrotMakra()
{
  TPluginActionMakra.cbSize = sizeof(PluginAction);
  TPluginActionMakra.pszName = (wchar_t*)L"AQQRestarterPrzypiszSkrotMakra";
  TPluginActionMakra.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginActionMakra.Position = 14;
  TPluginActionMakra.IconIndex = plugin_icon_idx;
  TPluginActionMakra.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginActionMakra.pszPopupName = (wchar_t*) L"popMacros";

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMakra));
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  //Katalog aktywnej kompozycji
  AnsiString PluginPath = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,(WPARAM)(hInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  if(!FileExists(PluginPath + "\\\\Icons\\\\AQQRestarter.png"))
  {
    //Katalog prywatny wtyczel
    PluginPath = (wchar_t*)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
    PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

    if(!FileExists(PluginPath + "\\\\AQQRestarter.png"))
    {
      //Wypakowanie ikon
      ExtractExe(ID_PNG,PluginPath + "\\\\AQQRestarter.png");
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(PluginPath + "\\\\AQQRestarter.png");
      plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
      //Usuniecie ikony
      DeleteFile("AQQRestarter.png");
    }
    else
    {
      //Przypisanie ikony
      wchar_t* plugin_icon_path = AnsiTowchar_t(PluginPath + "\\\\AQQRestarter.png");
      plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
    }
  }
  else
  {
    //Przypisanie ikony
    wchar_t* plugin_icon_path = AnsiTowchar_t(PluginPath + "\\\\Icons\\\\AQQRestarter.png");
    plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));
  }

  if(FileExists(PluginPath + "\\\\Restarter.exe"))
   DeleteFile("Restarter.exe");

  //Utworzenie serwisu restartu
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
  //Przypisanie skrutow
  PrzypiszSkrotMenu();
  PrzypiszSkrotMakra();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  TPluginLink.DestroyServiceFunction(AqqReStartService);
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMenu));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMakra));
 
  return 0;
}
//---------------------------------------------------------------------------
