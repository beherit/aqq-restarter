//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop    
#pragma argsused
#include "Aqq.h"
#include <memory> 
#include "ikonka.rh"
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}

//Utworzenie obiektow do struktur
PluginAction TPluginActionMenu;
PluginAction TPluginActionMakra;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

int plugin_icon_idx; //Zmienna do ikony

//Serwis restartu
int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  Application->Terminate();
  WinExec("aqq.exe", SW_SHOW);

  return 0;
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"AQQ Restarter";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,0,0);
  TPluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"";

  return &TPluginInfo;
}

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
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

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
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

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

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  //Wypakowanie ikon
  ExtractExe(ID_PNG,"AQQRestarter.png");

  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"AQQRestarter.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Usuniecie ikony
  DeleteFile("AQQRestarter.png");

  PrzypiszSkrotMenu();
  PrzypiszSkrotMakra();

  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  TPluginLink.DestroyServiceFunction(AqqReStartService);
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMenu));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMakra));
 
  return 0;
}
