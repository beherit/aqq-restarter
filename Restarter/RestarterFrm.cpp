//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "RestarterFrm.h"
#include <inifiles.hpp>
#include <tlhelp32.h>
#include <Registry.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRestarterForm *RestarterForm;
//---------------------------------------------------------------------------
__fastcall TRestarterForm::TRestarterForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

bool CloseProcess(DWORD ID)
{
 DWORD ExitCode;
 HWND hWnd;
 bool Result = true;

 if(ID)
 {
  hWnd = OpenProcess(PROCESS_ALL_ACCESS, true, ID);
  if(hWnd)
  {
   GetExitCodeProcess(hWnd, &ExitCode);
   Result = TerminateProcess(hWnd, ExitCode);
  }
  else return false;
 }
 else return false;

 CloseHandle(hWnd);
 
 return Result;
}
//---------------------------------------------------------------------------

String GetPathOfProces(DWORD PID)
{
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID);
  MODULEENTRY32 me32;
  Module32First(hSnapshot,&me32);

  CloseHandle(hSnapshot);

  return me32.szExePath;
}
//---------------------------------------------------------------------------

void __fastcall TRestarterForm::FormShow(TObject *Sender)
{
  Application->Minimize();
  ShowWindow(Application->Handle, SW_HIDE);
  //Odczyt PID
  TIniFile *Ini = new TIniFile(ExtractFilePath(Application->ExeName) + "\\\\Restarter.ini");
  int PID = Ini->ReadInteger("Restarter", "AQQPid", 0);
  AnsiString AQQPath = Ini->ReadString("Restarter", "AQQPath", "");
  AQQPath=StringReplace(AQQPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  delete Ini;
  if(PID!=0)
  {
    if(AQQPath=="")
    {
      //Pobieranie sciezki AQQ przez PID
      AnsiString AQQPath = GetPathOfProces(PID);
      AQQPath=StringReplace(AQQPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
    }
    //Restartowanie AQQ
    CloseProcess(PID);
    //Usuwanie pliku INI
    if(FileExists(ExtractFilePath(Application->ExeName) + "\\\\Restarter.ini"))
     DeleteFile(ExtractFilePath(Application->ExeName) + "\\\\Restarter.ini");
    //Czyszczenie Tray'a
    HWND hWnd = FindWindow("Shell_TrayWnd", NULL);
    hWnd = FindWindowEx(hWnd, NULL, "TrayNotifyWnd", NULL);
    hWnd = FindWindowEx(hWnd, NULL, "SysPager", NULL);
    hWnd = FindWindowEx(hWnd, NULL, "ToolbarWindow32", NULL);
    TRect rRect;
    GetWindowRect(hWnd,&rRect);
    for (LONG x = 0; x < rRect.right; x += 5)
     for (LONG y = 0; y < rRect.bottom; y += 5)
      SendMessage(hWnd,WM_MOUSEMOVE,0,(y << 16) + x);
    //W�aczenie AQQ
    ShellExecute(NULL, "open", AQQPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
  }
  //Wy��czanie programu
  Close();
}
//---------------------------------------------------------------------------
