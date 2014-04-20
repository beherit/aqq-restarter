//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

// Utworzenie obiektow do struktur
PluginAction TPluginAction;
PluginAction TPluginAction2;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  WinExec("aqq.exe", SW_SHOW);
  exit(EXIT_SUCCESS);

  return 0;
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"AQQRestarter";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(0,0,1,0);
  TPluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Prawa zastrze¿one, tylko dla autora.";
  TPluginInfo.Homepage = (wchar_t *)L"Brak";
  TPluginInfo.Flag = 0;
  TPluginInfo.ReplaceDefaultModule = 0;
  
  return &TPluginInfo;
}

void PrzypiszAkcje()
{
  TPluginAction.cbSize = sizeof(PluginAction);
  TPluginAction.pszName = (wchar_t*)L"AqqReStartServ";
  TPluginAction.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginAction.Position = 11;
  TPluginAction.IconIndex = 0;
  TPluginAction.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginAction.pszPopupName = (wchar_t*) L"muProgram";
  TPluginAction.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

void PrzypiszAkcje2()
{
  TPluginAction2.cbSize = sizeof(PluginAction);
  TPluginAction2.pszName = (wchar_t*)L"AqqReStartServ";
  TPluginAction2.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginAction2.Position = 14;
  TPluginAction2.IconIndex = 0;
  TPluginAction2.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginAction2.pszPopupName = (wchar_t*) L"popMacros";
  TPluginAction2.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginAction2));
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;
  PrzypiszAkcje();
  PrzypiszAkcje2();

  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  TPluginLink.DestroyServiceFunction(AqqReStartService);
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginAction));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginAction2));
 
  return 0;
}
