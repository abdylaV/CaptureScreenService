// CaptureScreenService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>

#include <string>

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

//#define SERVICE_NAME  _T("CaptureScreenService")

#include "../Tools/Log.h"

int _tmain(int argc, TCHAR *argv[])
{
	OutputDebugString(_T("CaptureScreenService: Main: Entry"));

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ /*SERVICE_NAME*/const_cast<wchar_t*>(strNameService), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};

	//if (argc < 2)
	//{
	//	if (!StartServiceCtrlDispatcher(ServiceTable))
	//	{
	//		DWORD f = GetLastError();
	//	}
	//}
	//else if (wcscmp(argv[1], L"install")
	//{
	//	//InstallService();
	//}

	std::wstring strStatus;

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		strStatus = L"CaptureScreenService: Main: StartServiceCtrlDispatcher returned error = ";
		strStatus += std::to_wstring(GetLastError());

		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());

		return GetLastError();
	}

	strStatus = L"CaptureScreenService: Main: Exit";
	
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	return 0;
}


VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	DWORD Status = E_FAIL;

	std::wstring strStatus;

	strStatus = L"CaptureScreenService: ServiceMain: Entry";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	g_StatusHandle = RegisterServiceCtrlHandler(/*SERVICE_NAME*/strNameService, ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
	{
		strStatus = L"CaptureScreenService: ServiceMain: RegisterServiceCtrlHandler returned error = ";
		strStatus += std::to_wstring(GetLastError());
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());

		goto EXIT;
	}

	// Tell the service controller we are starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;// | SERVICE_INTERACTIVE_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		strStatus = L"CaptureScreenService: ServiceMain: SetServiceStatus returned error = ";
		strStatus += std::to_wstring(GetLastError());
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());
	}

	/*
	* Perform tasks neccesary to start the service here
	*/

	strStatus = L"CaptureScreenService: ServiceMain: Performing Service Start Operations";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	// Create stop event to wait on later.
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL)
	{
		strStatus = L"CaptureScreenService: ServiceMain: CreateEvent(g_ServiceStopEvent) returned error = ";
		strStatus += std::to_wstring(GetLastError());
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			strStatus = L"CaptureScreenService: ServiceMain: SetServiceStatus returned error = ";
			strStatus += std::to_wstring(GetLastError());
			logTo(strStatus.c_str());

			OutputDebugString(strStatus.c_str());
		}
		goto EXIT;
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		strStatus = L"CaptureScreenService: ServiceMain: SetServiceStatus returned error = ";
		strStatus += std::to_wstring(GetLastError());
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());
	}

	// Start the thread that will perform the main task of the service
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	strStatus = L"CaptureScreenService: ServiceMain: Waiting for Worker Thread to complete";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	// Wait until our worker thread exits effectively signaling that the service needs to stop
	WaitForSingleObject(hThread, INFINITE);

	strStatus = L"CaptureScreenService: ServiceMain: Worker Thread Stop Event signaled";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	/*
	* Perform any cleanup tasks
	*/

	strStatus = L"CaptureScreenService: ServiceMain: Performing Cleanup Operations";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	CloseHandle(g_ServiceStopEvent);

	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		strStatus = L"CaptureScreenService: ServiceMain: SetServiceStatus returned error = ";
		strStatus += std::to_wstring(GetLastError());
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());
	}

EXIT:
	strStatus = L"CaptureScreenService: ServiceMain: Exit";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	return;
}


VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	std::wstring strStatus;

	strStatus = L"CaptureScreenService: ServiceCtrlHandler: Entry";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		strStatus = L"CaptureScreenService: ServiceCtrlHandler: SERVICE_CONTROL_STOP Request";
		logTo(strStatus.c_str());

		OutputDebugString(strStatus.c_str());

		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		* Perform tasks neccesary to stop the service here
		*/

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			strStatus = L"CaptureScreenService: ServiceCtrlHandler: SetServiceStatus returned error = ";
			strStatus += std::to_wstring(GetLastError());
			logTo(strStatus.c_str());

			OutputDebugString(strStatus.c_str());
		}

		// This will signal the worker thread to start shutting down
		SetEvent(g_ServiceStopEvent);

		break;

	default:
		break;
	}

	strStatus = L"CaptureScreenService: ServiceCtrlHandler: Exit";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());
}

#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

int StartProcess(const wchar_t * progName)
{
	HANDLE hToken;
	
	wchar_t errStr[100];
	DWORD dwCreationFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	int k = WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &hToken);

	ZeroMemory(&si, sizeof(STARTUPINFO));

	si.cb = sizeof(STARTUPINFO);
	//si.lpDesktop = L"winsta0\\default";
	si.wShowWindow = SW_HIDE;

	ZeroMemory(&pi, sizeof(pi));

	FreeConsole();

	if (!CreateProcessAsUser(
		hToken,
		progName,
		/*fileLocationLp*/NULL,
		NULL,
		NULL,
		FALSE,
		dwCreationFlag,
		NULL,
		NULL,
		&si,
		&pi
		))
	{
		swprintf_s(errStr, L"CreateProcessAsUser Failed %d\n", GetLastError());
		logTo(errStr);

		exit(1);
	}
	else
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hToken);
	}

	return 0;
}

//
//sc.exe create "CaptureScreenService" binPath="C:\NVIDIA\DisplayDriver\361.91\Win8_WinVista_Win7_64\International\NVBar\CaptureScreenService.exe"
//[SC] CreateService SUCCESS
//
//sc.exe delete "CaptureScreenService"
//[SC] DeleteService SUCCESS
//

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	std::wstring strStatus;

	strStatus = L"CaptureScreenService: ServiceWorkerThread: Entry";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		Sleep(1000);

		//logTo("ServiceWorkerThread\n");

		StartProcess(getStrPath(str_WindowSaveScreen).c_str());
	}

	strStatus = L"CaptureScreenService: ServiceWorkerThread: Exit";
	logTo(strStatus.c_str());

	OutputDebugString(strStatus.c_str());

	return ERROR_SUCCESS;
}

