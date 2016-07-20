// WindowSaveScreen.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "WindowSaveScreen.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#include <stdio.h>
#include <iostream>
#include <string>

#include <time.h>

#include <objidl.h>
#include <gdiplus.h>

#include <Lmcons.h>

#include <vector>

using namespace std;

#pragma comment(lib, "GdiPlus.lib") /* наш многострадальный lib-файл */
using namespace Gdiplus; /* как хочешь, но мне не в кайф постоянно писать Gdiplus:: */

static const GUID png =
{ 0x557cf406, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };

enum type_mode_screen { MAIN_SCREEN, ALL_SCREENS, SEPARATE_SCREENS };

struct SizeMonitor
{
public:
	int x, y, Width, Height;
public:
	SizeMonitor(int x_, int y_, int Width_, int Height_)
	{
		x = x_;
		y = y_;
		Width = Width_;
		Height = Height_;
	}
};

std::vector<HMONITOR> g_Monitors;
BOOL CALLBACK EnumMonitors(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM lParam)
{
	g_Monitors.push_back(hMonitor);

	return TRUE;
}

int CreatePicturePNG(wchar_t *filename, SizeMonitor size_param)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HDC scrdc = NULL, memdc;
	HBITMAP membit;

	// Получаем HDC рабочего стола
	// Параметр HWND для рабочего стола всегда равен нулю.
	scrdc = GetDC(NULL);

	if (scrdc == NULL)
		return 0;

	// Определяем разрешение экрана
	int x, y, Width, Height;
	
	x = size_param.x;
	y = size_param.y;
	Width = size_param.Width;
	Height = size_param.Height;

	/*

	if (mode == MAIN_SCREEN)
	{
		x = y = 0;
		Height = GetSystemMetrics(SM_CYSCREEN);
		Width = GetSystemMetrics(SM_CXSCREEN);
	}
	else
	if (mode == ALL_SCREENS)
	{
		x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		y = GetSystemMetrics(SM_YVIRTUALSCREEN);
		Width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		Height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	*/

	// Создаем новый DC, идентичный десктоповскому и битмап размером с экран.
	memdc = CreateCompatibleDC(scrdc);
	membit = CreateCompatibleBitmap(scrdc, Width, Height);
	SelectObject(memdc, membit);

	// Улыбаемся... Снято!
	//BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
	BOOL ok = StretchBlt(memdc, 0, 0, Width, Height, scrdc, x, y, Width, Height, SRCCOPY);

	HBITMAP hBitmap;
	hBitmap = (HBITMAP)SelectObject(memdc, membit);
	Gdiplus::Bitmap bitmap(hBitmap, NULL);
	bitmap.Save(filename, &png);

	DeleteObject(hBitmap);

	//GdiplusShutdown(gdiplusToken);
	return 0;
}

#include "../Tools/Log.h"

// D:\111\ffmpeg-20160622-e0faad8-win64-static\bin\ffmpeg.exe -r 5 -i "D:\testfiles\2016_5_24\img_%06d.png" -vcodec libx264 "D:\testfiles\out.mp4"
// D:\111\ffmpeg-20160622-e0faad8-win64-static\bin\ffmpeg.exe - i "D:\testfiles\input.mp4" - i "E:\fff\Music\input.mp3" - c copy - map 0:0 - map 1 : 0 "D:\testfiles\output.mp4"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	type_mode_screen mode_screen_save = MAIN_SCREEN;
	//type_mode_screen mode_screen_save = ALL_SCREENS;
	//type_mode_screen mode_screen_save = SEPARATE_SCREENS;

	wchar_t filename[255];

	time_t rawtime = time(nullptr);
	tm timeinfo;
	char str1[32];
	char str2[32];

	errno_t result = localtime_s(&timeinfo, &rawtime);

	wchar_t username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserNameW(username, &username_len);

	std::wstring strPathDir;

	strPathDir = strPathOutputDir;
	strPathDir += L"\\";
	strPathDir += username;

	if (CreateDirectory(strPathDir.c_str(), NULL))
	{
		logTo(L"WindowSaveScreen: username directory create");
	}
	else
	{
		//logTo(L"WindowSaveScreen: username directory was created");
	}

	// http://www.cplusplus.com/reference/ctime/tm/

	strPathDir += L"\\";
	strPathDir += std::to_wstring(timeinfo.tm_year + 1900);
	strPathDir += L"_";
	strPathDir += std::to_wstring(timeinfo.tm_mon + 1);
	strPathDir += L"_";
	strPathDir += std::to_wstring(timeinfo.tm_mday);

	if (CreateDirectory(strPathDir.c_str(), NULL))
	{
		logTo(L"WindowSaveScreen: directory create");
	}
	else
	{
		//logTo(L"WindowSaveScreen: directory was created");
	}

	int iCounter = GetCountImg(strPathDir);

	wchar_t strTime[256];
	swprintf_s(strTime, 256, L"img_%06d.png", iCounter);

	std::wstring strPathFile;
	strPathFile = strPathDir;
	strPathFile += L"\\";
	strPathFile += strTime;

	int x, y, Width, Height;
	std::vector<SizeMonitor> list_monitors_size;

	if (mode_screen_save == MAIN_SCREEN)
	{
		x = y = 0;
		Height = GetSystemMetrics(SM_CYSCREEN);
		Width = GetSystemMetrics(SM_CXSCREEN);
		
		list_monitors_size.push_back(SizeMonitor(x, y, Width, Height));
	}
	else if (mode_screen_save == ALL_SCREENS)
	{
		x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		y = GetSystemMetrics(SM_YVIRTUALSCREEN);
		Width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		Height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		list_monitors_size.push_back(SizeMonitor(x, y, Width, Height));
	}
	else if (mode_screen_save == SEPARATE_SCREENS)
	{
		EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)EnumMonitors, (LPARAM)nullptr);

		bool res = true;
		MONITORINFOEX monitorInfo;
		DEVMODE dm;

		for (int i = 0; i < g_Monitors.size(); i++)
		{
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo((HMONITOR)g_Monitors[i], &monitorInfo);

			ZeroMemory(&dm, sizeof(dm));
			res = EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
			if (res) 
			{
				list_monitors_size.push_back(SizeMonitor(dm.dmPosition.x, dm.dmPosition.y, dm.dmPelsWidth, dm.dmPelsHeight));
			}
		}
	}

	for (int i = 0; i < list_monitors_size.size(); i++)
	{
		std::wstring strPathFile;
		strPathFile = strPathDir;
		
		if (list_monitors_size.size() > 1)
		{
			strPathFile += L"\\";
			strPathFile += std::to_wstring(static_cast<long long>(i + 1));

			if (CreateDirectory(strPathFile.c_str(), NULL))
			{
				//logTo(L"WindowSaveScreen: directory create");
			}
			else
			{
				//logTo(L"WindowSaveScreen: directory2 was created");
			}
		}

		strPathFile += L"\\";
		strPathFile += strTime;

		CreatePicturePNG(const_cast<wchar_t*>(strPathFile.c_str()), list_monitors_size[i]);
	}

	SetCountImg(strPathDir, ++iCounter);

	return 0;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSAVESCREEN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSAVESCREEN));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSAVESCREEN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSAVESCREEN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
