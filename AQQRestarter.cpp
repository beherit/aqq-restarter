//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include "Aqq.h"
#include <memory>
#include "ikonka.rh"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}

// Utworzenie obiektow do struktur
PluginAction TPluginActionMenu;
PluginAction TPluginActionMakra;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

int plugin_icon_idx;

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
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(0,0,2,0);
  TPluginInfo.Description = (wchar_t *)L"Szybki restart AQQ z pozycji menu";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Prawa zastrzeżone, tylko dla autora.";
  TPluginInfo.Homepage = (wchar_t *)L"Brak";
  TPluginInfo.Flag = 0;
  TPluginInfo.ReplaceDefaultModule = 0;
  
  return &TPluginInfo;
}

void PrzypiszAkcje()
{
  TPluginActionMenu.cbSize = sizeof(PluginAction);
  TPluginActionMenu.pszName = (wchar_t*)L"AqqReStartServ";
  TPluginActionMenu.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginActionMenu.Position = 11;
  TPluginActionMenu.IconIndex = plugin_icon_idx;
  TPluginActionMenu.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginActionMenu.pszPopupName = (wchar_t*) L"muProgram";
  TPluginActionMenu.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMenu));
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

void PrzypiszAkcje2()
{
  TPluginActionMakra.cbSize = sizeof(PluginAction);
  TPluginActionMakra.pszName = (wchar_t*)L"AqqReStartServ";
  TPluginActionMakra.pszCaption = (wchar_t*) L"Zrestartuj AQQ";
  TPluginActionMakra.Position = 14;
  TPluginActionMakra.IconIndex = plugin_icon_idx;
  TPluginActionMakra.pszService = (wchar_t*) L"serwis_aqqrestart";
  TPluginActionMakra.pszPopupName = (wchar_t*) L"popMacros";
  TPluginActionMakra.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginActionMakra));
  TPluginLink.CreateServiceFunction(L"serwis_aqqrestart",AqqReStartService);
}

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  //wypakowanie ikonki
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID_PNG), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  if(MemoryHandle == NULL) return 0;

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;
  stream->SaveToFile("AQQRestarter.png");
  //koniec

  wchar_t* plugin_icon_path = L"AQQRestarter.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  DeleteFile("AQQRestarter.png");

  PrzypiszAkcje();
  PrzypiszAkcje2();

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
