//---------------------------------------------------------------------------
// Copyright (C) 2008-2014 Krzysztof Grochocki
//
// This file is part of AQQ Restarter
//
// AQQ Restarter is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// AQQ Restarter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <PluginAPI.h>
#include <IdHashMessageDigest.hpp>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall SystemRestart(WPARAM wParam, LPARAM lParam);
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
  return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Tworzenie elementu w menu Program
void BuildProgramItem()
{
  //Ustalanie pozycji elementu "Zakoncz"
  TPluginItemDescriber PluginItemDescriber;
  ZeroMemory(&PluginItemDescriber,sizeof(TPluginItemDescriber));
  PluginItemDescriber.cbSize = sizeof(TPluginItemDescriber);
  PluginItemDescriber.ParentName = (wchar_t*)L"muProgram";
  PluginItemDescriber.Name = (wchar_t*)L"Zakocz2";
  TPluginAction Action = *(PPluginAction)PluginLink.CallService(AQQ_CONTROLS_GETPOPUPMENUITEM,0,(LPARAM)(&PluginItemDescriber));
  //Wypelnianie struktury
  TPluginAction PluginActionProgramItem;
  ZeroMemory(&PluginActionProgramItem,sizeof(TPluginAction));
  PluginActionProgramItem.cbSize = sizeof(TPluginAction);
  PluginActionProgramItem.pszName = L"AQQRestarterProgramItem";
  PluginActionProgramItem.pszCaption = L"Zrestartuj";
  PluginActionProgramItem.Position = Action.Position - 1;
  PluginActionProgramItem.IconIndex = 19;
  PluginActionProgramItem.pszService = L"sAQQRestarterSystemRestart";
  PluginActionProgramItem.pszPopupName = L"muProgram";
  //Tworzenie elementu w interfejsie
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionProgramItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu z menu makr
void DestroyProgramItem()
{
  //Wypelnianie struktury
  TPluginAction PluginActionProgramItem;
  ZeroMemory(&PluginActionProgramItem,sizeof(TPluginAction));
  PluginActionProgramItem.cbSize = sizeof(TPluginAction);
  PluginActionProgramItem.pszName = L"AQQRestarterProgramItem";
  //Usuwanie elementu z interfejsu
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionProgramItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu w menu makr
void BuildMacrosItem()
{
  //Wypelnianie struktury
  TPluginAction PluginActionMacrosItem;
  ZeroMemory(&PluginActionMacrosItem,sizeof(TPluginAction));
  PluginActionMacrosItem.cbSize = sizeof(TPluginAction);
  PluginActionMacrosItem.pszName = L"AQQRestarterMacrosItem";
  PluginActionMacrosItem.pszCaption = L"Zrestartuj";
  PluginActionMacrosItem.Position = 15;
  PluginActionMacrosItem.IconIndex = 19;
  PluginActionMacrosItem.pszService = L"sAQQRestarterSystemRestart";
  PluginActionMacrosItem.pszPopupName = L"popMacros";
  //Tworzenie elementu w interfejsie
  PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&PluginActionMacrosItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu z menu makr
void DestroyMacrosItem()
{
  //Wypelnianie struktury
  TPluginAction PluginActionMacrosItem;
  ZeroMemory(&PluginActionMacrosItem,sizeof(TPluginAction));
  PluginActionMacrosItem.cbSize = sizeof(TPluginAction);
  PluginActionMacrosItem.pszName = L"AQQRestarterMacrosItem";
  //Usuwanie elementu z interfejsu
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&PluginActionMacrosItem));
}
//---------------------------------------------------------------------------

//Serwis restartu
INT_PTR __stdcall SystemRestart(WPARAM wParam, LPARAM lParam)
{
  //Restart AQQ z dzierzawa uprawnien
  PluginLink.CallService(AQQ_SYSTEM_RESTART,1,0);

  return 0;
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

extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Wypakiwanie ikonki AQQRestarter.dll.png
  //9C5C93C35D61AF3149D8382A07E131FB
  if(!DirectoryExists(GetPluginUserDir()+"\\\\Shared"))
   CreateDir(GetPluginUserDir()+"\\\\Shared");
  if(!FileExists(GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png"))
   ExtractRes((GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
  else if(MD5File(GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png")!="9C5C93C35D61AF3149D8382A07E131FB")
   ExtractRes((GetPluginUserDir()+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
  //Tworzenie serwisu restartu
  PluginLink.CreateServiceFunction(L"sAQQRestarterSystemRestart",SystemRestart);
  //Tworzenie elementow w interfejsie
  BuildProgramItem();
  BuildMacrosItem();
  
  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
  //Usuwanie elementow z interfejsu
  DestroyProgramItem();
  DestroyMacrosItem();
  //Usuwanie serwisu restartu
  PluginLink.DestroyServiceFunction(SystemRestart);

  return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = L"AQQ Restarter";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,5,1,0);
  PluginInfo.Description = L"Szybki restart komunikatora z pozycji menu Program, menu kompaktowego lub menu makr z zasobnika systemowego.";
  PluginInfo.Author = L"Krzysztof Grochocki";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki";
  PluginInfo.Homepage = L"http://beherit.pl";
  PluginInfo.Flag = 0;
  PluginInfo.ReplaceDefaultModule = 0;

  return &PluginInfo;
}
//---------------------------------------------------------------------------
