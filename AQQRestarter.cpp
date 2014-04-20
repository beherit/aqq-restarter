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

AnsiString UserPath;
int IloscProfili=0;
int plugin_icon_idx; //Zmienna do ikony

//szukanie profili
void FindDir(String Dir) //Funkcja szukaj¹ca profile
{
  TSearchRec sr;

  if(FindFirst(Dir + "*.*", faAnyFile, sr) == 0)
     {
      do{
         if(((sr.Attr & faDirectory) > 0) & (sr.Name != ".") & (sr.Name != ".."))
         {
            IloscProfili++;
         }
        }
        while(FindNext(sr) == 0);
        FindClose(sr);
     }
}

//Serwis restartu
int __stdcall AqqReStartService (WPARAM, LPARAM)
{
  if (IloscProfili==1)
  {
    WinExec("aqq.exe", SW_SHOW);
    exit(EXIT_SUCCESS);
  }
  else
  {
    WinExec("aqq.exe", SW_SHOW);
    TPluginLink.CallService(AQQ_SYSTEM_RUNACTION,0,(LPARAM)(L"aExit"));
  }

  return 0;
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t*)L"AQQ Restarter";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(0,0,4,4);
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

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  //Katalog uzytkownika
  UserPath = (wchar_t *)(TPluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(hInstance),0));

  //Usuwanie nazwy profilu ze sciezki
  AnsiString temp = "\\";
  int x = UserPath.LastDelimiter(temp);
  int y = UserPath.Length();
  UserPath.Delete(x, y);

  UserPath = StringReplace(UserPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  UserPath = UserPath + "\\\\";

  //Szukanie ilosci profili
  FindDir(UserPath);

  //Wypakowanie ikony
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID_PNG), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  if(MemoryHandle == NULL) return 0;

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;
  stream->SaveToFile("AQQRestarter.png");
  //Wypakowanie ikony - Koniec

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
