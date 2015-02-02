//---------------------------------------------------------------------------
// Copyright (C) 2008-2015 Krzysztof Grochocki
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
#include <IdHashMessageDigest.hpp>
#include <PluginAPI.h>
#include <LangAPI.hpp>
#pragma hdrstop

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//Struktury-glowne-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam);
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
	TPluginAction ProgramItem;
	ZeroMemory(&ProgramItem,sizeof(TPluginAction));
	ProgramItem.cbSize = sizeof(TPluginAction);
	ProgramItem.pszName = L"AQQRestarterProgramItem";
	ProgramItem.pszCaption = GetLangStr("Restart").w_str();
	ProgramItem.Position = Action.Position - 1;
	ProgramItem.IconIndex = 19;
	ProgramItem.pszService = L"sAQQRestarterSystemRestart";
	ProgramItem.pszPopupName = L"muProgram";
	//Tworzenie elementu w interfejsie
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&ProgramItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu z menu Program
void DestroyProgramItem()
{
	//Wypelnianie struktury
	TPluginAction ProgramItem;
	ZeroMemory(&ProgramItem,sizeof(TPluginAction));
	ProgramItem.cbSize = sizeof(TPluginAction);
	ProgramItem.pszName = L"AQQRestarterProgramItem";
	//Usuwanie elementu z interfejsu
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&ProgramItem));
}
//---------------------------------------------------------------------------

//Aktualizacja elementu w menu Program
void RebuildProgramItem()
{
	TPluginActionEdit ProgramItem;
	ZeroMemory(&ProgramItem,sizeof(TPluginActionEdit));
	ProgramItem.cbSize = sizeof(TPluginActionEdit);
	ProgramItem.pszName = L"AQQRestarterProgramItem";
	ProgramItem.Caption = GetLangStr("Restart").w_str();
	ProgramItem.IconIndex = 19;
	ProgramItem.Enabled = true;
	ProgramItem.Visible = true;
	ProgramItem.Checked = false;
	PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&ProgramItem));
}
//---------------------------------------------------------------------------

//Tworzenie elementu w menu makr
void BuildMacrosItem()
{
	//Wypelnianie struktury
	TPluginAction MacrosItem;
	ZeroMemory(&MacrosItem,sizeof(TPluginAction));
	MacrosItem.cbSize = sizeof(TPluginAction);
	MacrosItem.pszName = L"AQQRestarterMacrosItem";
	MacrosItem.pszCaption = GetLangStr("Restart").w_str();
	MacrosItem.Position = 15;
	MacrosItem.IconIndex = 19;
	MacrosItem.pszService = L"sAQQRestarterSystemRestart";
	MacrosItem.pszPopupName = L"popMacros";
	//Tworzenie elementu w interfejsie
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&MacrosItem));
}
//---------------------------------------------------------------------------

//Usuwanie elementu z menu makr
void DestroyMacrosItem()
{
	//Wypelnianie struktury
	TPluginAction MacrosItem;
	ZeroMemory(&MacrosItem,sizeof(TPluginAction));
	MacrosItem.cbSize = sizeof(TPluginAction);
	MacrosItem.pszName = L"AQQRestarterMacrosItem";
	//Usuwanie elementu z interfejsu
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&MacrosItem));
}
//---------------------------------------------------------------------------

//Aktualizacja elementu w menu makr
void RebuildMacrosItem()
{
	TPluginActionEdit MacrosItem;
	ZeroMemory(&MacrosItem,sizeof(TPluginActionEdit));
	MacrosItem.cbSize = sizeof(TPluginActionEdit);
	MacrosItem.pszName = L"AQQRestarterMacrosItem";
	MacrosItem.Caption = GetLangStr("Restart").w_str();
	MacrosItem.IconIndex = 19;
	MacrosItem.Enabled = true;
	MacrosItem.Visible = true;
	MacrosItem.Checked = false;
	PluginLink.CallService(AQQ_CONTROLS_EDITPOPUPMENUITEM,0,(LPARAM)(&MacrosItem));
}
//---------------------------------------------------------------------------

//Hook na zmiane lokalizacji
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam)
{
	//Czyszczenie cache lokalizacji
	ClearLngCache();
	//Pobranie sciezki do katalogu prywatnego uzytkownika
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)lParam;
	LangPath = PluginUserDir + "\\\\Languages\\\\AQQRestarter\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\AQQRestarter\\\\" + LangCode + "\\\\";
	}
	//Aktualizacja lokalizacji w elementach intefejsu
	RebuildProgramItem();
	RebuildMacrosItem();

	return 0;
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
	else return 0;
}
//---------------------------------------------------------------------------

extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
	//Linkowanie wtyczki z komunikatorem
	PluginLink = *Link;
	//Sciezka folderu prywatnego wtyczek
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Tworzenie katalogow lokalizacji
	if(!DirectoryExists(PluginUserDir+"\\\\Languages"))
		CreateDir(PluginUserDir+"\\\\Languages");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\AQQRestarter"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\AQQRestarter");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL");
	//Wypakowanie plikow lokalizacji
	//6F935720BBD0806F3E100D222C14D1B7
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN\\\\Const.lng")!="6F935720BBD0806F3E100D222C14D1B7")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	//BB02FA50064DFCFD997520A90273642A
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL\\\\Const.lng")!="BB02FA50064DFCFD997520A90273642A")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\AQQRestarter\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETLANGCODE,0,0);
	LangPath = PluginUserDir + "\\\\Languages\\\\AQQRestarter\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\AQQRestarter\\\\" + LangCode + "\\\\";
	}
	//Wypakiwanie ikonki AQQRestarter.dll.png
	//9C5C93C35D61AF3149D8382A07E131FB
	if(!DirectoryExists(PluginUserDir+"\\\\Shared"))
		CreateDir(PluginUserDir+"\\\\Shared");
	if(!FileExists(PluginUserDir+"\\\\Shared\\\\AQQRestarter.dll.png"))
		ExtractRes((PluginUserDir+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Shared\\\\AQQRestarter.dll.png")!="9C5C93C35D61AF3149D8382A07E131FB")
		ExtractRes((PluginUserDir+"\\\\Shared\\\\AQQRestarter.dll.png").w_str(),L"SHARED",L"DATA");
	//Hook na zmiane lokalizacji
	PluginLink.HookEvent(AQQ_SYSTEM_LANGCODE_CHANGED,OnLangCodeChanged);
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
	//Wyladowanie wszystkich hookow
	PluginLink.UnhookEvent(OnLangCodeChanged);

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"AQQ Restarter";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(2,6,0,0);
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
