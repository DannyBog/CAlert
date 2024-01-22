#define INITGUID
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <roapi.h>
#include <winhttp.h>
#include <stdint.h>
#include <stdbool.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "runtimeobject.lib")

#ifdef _DEBUG
#define ASSERT(condition) do {if (!(condition)) __debugbreak();} while (0)
#else
#define ASSERT(condition) (void)(condition)
#endif

#define HR(hr) do {HRESULT _hr = (hr); ASSERT(SUCCEEDED(_hr));} while (0)

#include "bog\bog_types.h"
#include "bog\bog_stringw.h"
#include "windows_toast.h"
#include "windows_json.h"

#define APP_NAME		L"CAlert"
#define NOTIFY_APPID	L"CAlert.CAlert"

#define WM_APP_CLICKED	(WM_USER + 0)
#define WM_APP_RUNNING	(WM_USER + 1)
#define CMD_HISTORY		(WM_USER + 2)
#define CMD_ABOUT		(WM_USER + 3)
#define CMD_EXIT		(WM_USER + 4)

#define MAX_STRING_LENGTH 256
#define MAX_ALERTS_COUNT 1000

#define QUERY_DELAY 1000

typedef struct {
	wchar_t date[20];
	wchar_t title[MAX_STRING_LENGTH];
	wchar_t data[MAX_STRING_LENGTH];
} alert;

static HINTERNET gSession;
static WindowsToast gToast;
static HICON gIcon, gIconDisconnected;
static alert gAlerts[MAX_ALERTS_COUNT];

static void AddTrayIcon(HWND hWindow, HICON hIcon) {
	NOTIFYICONDATAW nid = {
		.cbSize = sizeof(nid),
		.hWnd = hWindow,
		.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
		.uCallbackMessage = WM_APP_CLICKED,
		.hIcon = hIcon
	};
	
	BOGStringCopyToIndexW(nid.szTip, APP_NAME, ARRAYSIZE(nid.szTip));
	Shell_NotifyIconW(NIM_ADD, &nid);
}

static void RemoveTrayIcon(HWND hWindow) {
	NOTIFYICONDATAW nid = {
		.cbSize = sizeof(nid),
		.hWnd = hWindow,
	};
	
	Shell_NotifyIconW(NIM_DELETE, &nid);
}

static void UpdateTrayIcon(HWND hWindow, HICON hIcon) {
	NOTIFYICONDATAW nid = {
		.cbSize = sizeof(nid),
		.hWnd = hWindow,
		.uFlags = NIF_ICON,
		.hIcon = hIcon
	};
	
	Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void ShowTrayIcon(HWND hWindow, HMENU hMenu) {
	POINT pt;
	GetCursorPos(&pt);
					
	SetForegroundWindow(hWindow);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWindow, 0);
	PostMessage(hWindow, WM_NULL, 0, 0);
}

static void ShowTrayMessage(HWND hWindow, u32 info, wchar_t *msg) {
	NOTIFYICONDATAW nid = {
		.cbSize = sizeof(nid),
		.hWnd = hWindow,
		.uFlags = NIF_INFO,
		.dwInfoFlags = info
	};
	
	BOGStringCopyToIndexW(nid.szInfo, msg, ARRAYSIZE(nid.szInfo));
	BOGStringCopyToIndexW(nid.szInfoTitle, APP_NAME, ARRAYSIZE(nid.szInfoTitle));
	Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static s32 GetAlerts(HWND hWindow, wchar_t *url) {
	s32 alertsNum = 0;
	wchar_t domain[MAX_PATH] = {0};
	wchar_t path[MAX_PATH] = {0};
	BOGStringCopyToCharW(domain, url, L'/');
	BOGStringCopyFromCharW(path, url, L'/');
	
	if (gSession) {
		HINTERNET connection = WinHttpConnect(gSession, domain, INTERNET_DEFAULT_HTTPS_PORT, 0);
		if (connection) {
			HINTERNET request = WinHttpOpenRequest(connection, L"GET",
												   path,
												   0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
												   WINHTTP_FLAG_SECURE);
			if (request) {
				WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA,
								   0, 0, 0);
				if (WinHttpReceiveResponse(request, 0)) {
					char buffer[65536];
					uint32_t bytesRead;
					uint32_t bytesReadTotal = 0;
					uint32_t bufferSize = sizeof(buffer);
					
					while (bytesReadTotal < bufferSize) {
						if (!WinHttpReadData(request,
											 buffer + bytesReadTotal, bufferSize - bytesReadTotal,
											 (unsigned long *) &bytesRead) || !bytesRead) {
							break;
						}
						bytesReadTotal += bytesRead;
					}
					
					json_array *jsonArr = JsonArrayParse(buffer, bytesReadTotal);
					if (jsonArr) {
						alertsNum = JsonArrayGetLength(jsonArr);
						for (u32 i = 0; i < (u32) alertsNum; ++i) {
							json_object *jsonObj = JsonArrayGetObject(jsonArr, i);
							if (jsonObj) {
								HSTRING hDate = JsonObjectGetString(jsonObj, JsonCSTR("alertDate"));
								HSTRING hTitle = JsonObjectGetString(jsonObj, JsonCSTR("title"));
								HSTRING hData = JsonObjectGetString(jsonObj, JsonCSTR("data"));
								
								wchar_t *date = (wchar_t *) WindowsGetStringRawBuffer(hDate, 0);
								wchar_t *title = (wchar_t *) WindowsGetStringRawBuffer(hTitle, 0);
								wchar_t *data = (wchar_t *) WindowsGetStringRawBuffer(hData, 0);
								
								BOGStringCopyW(gAlerts[i].date, date);
								BOGStringCopyW(gAlerts[i].title, title);
								BOGStringCopyW(gAlerts[i].data, data);
								
								WindowsDeleteString(hDate);
								WindowsDeleteString(hTitle);
								WindowsDeleteString(hData);
							}
							JsonRelease(jsonObj);
						}
					} else {
						alertsNum = -1;
					}
					JsonRelease(jsonArr);
				} else {
					alertsNum = -1;
				}
				WinHttpCloseHandle(request);
			}
			WinHttpCloseHandle(connection);
		}
	}
	
	if (alertsNum != -1) {
		UpdateTrayIcon(hWindow, gIcon);
	} else {
		UpdateTrayIcon(hWindow, gIconDisconnected);
	}
	
	return alertsNum;
}

static void ShowAlertNotification(alert *alert) {
	wchar_t imagePath[MAX_PATH] = L"C:\\Windows\\System32\\SecurityAndMaintenance.png";
	
	for (wchar_t *path = imagePath; *path; path++) {
		if (*path == '\\') *path = '/';
	}

	wchar_t xml[4096];
	u32 xmlLength = 0;

	xmlLength = StrCatChainW(xml, ARRAYSIZE(xml), xmlLength,
							 L"<toast duration=\"short\"><visual><binding template=\"ToastGeneric\">"
							 L"<image placement=\"appLogoOverride\" src=\"file:///");
	xmlLength = StrCatChainW(xml, ARRAYSIZE(xml), xmlLength, imagePath);
	xmlLength = StrCatChainW(xml, ARRAYSIZE(xml), xmlLength,
							 L"\"/>"
							 L"<text hint-maxLines=\"1\">{date}</text>"
							 L"<text>{title}</text>"
							 L"<text>{data}</text>"
							 L"</binding></visual></toast>");

	wchar_t *data[][2] = {
		{L"date",	alert->date},
		{L"title",	alert->title},
		{L"data",	alert->data}
	};
	
	void *notification = WindowsToast_Create(&gToast, xml, xmlLength, data, ARRAYSIZE(data));
	WindowsToast_Show(&gToast, notification);
	WindowsToast_Release(&gToast, notification);
}

static void ParseDate(wchar_t *date, SYSTEMTIME *st) {
	wchar_t temp1 = date[4];
	date[4] = L'\0';
	st->wYear = (int16_t) BOGStringToIntW(date);
	
	wchar_t temp2 = date[7];
	date[7] = L'\0';
	st->wMonth = (int16_t) BOGStringToIntW(date + 5);
	
	date[4] = temp1;
	temp1 = date[10];
	date[10] = L'\0';
	st->wDay = (int16_t) BOGStringToIntW(date + 8);
	
	date[7] = temp2;
	temp2 = date[13];
	date[13] = L'\0';
	st->wHour = (int16_t) BOGStringToIntW(date + 11);
	
	date[10] = temp1;
	temp1 = date[16];
	date[16] = L'\0';
	st->wMinute = (int16_t) BOGStringToIntW(date + 14);
	
	date[13] = temp2;
	st->wSecond = (int16_t) BOGStringToIntW(date + 17);
	
	date[16] = temp1;
}

static DWORD WINAPI AlertsThread(LPVOID lpParameter) {
	HR(RoInitialize(RO_INIT_SINGLETHREADED));
	WindowsToast_Init(&gToast, APP_NAME, NOTIFY_APPID);
	WindowsToast_HideAll(&gToast, NOTIFY_APPID);
	
	HWND hWindow = (HWND) lpParameter;
	u32 alertsNum = GetAlerts(hWindow, L"www.oref.org.il/WarningMessages/History/AlertsHistory.json");
	alert currentAlert = gAlerts[0];
	alert previousAlert = currentAlert;
	
	for (;;) {
		alertsNum = GetAlerts(hWindow, L"www.oref.org.il/WarningMessages/History/AlertsHistory.json");
		if (alertsNum) {
			currentAlert = gAlerts[0];
				
			SYSTEMTIME st1 = {0};
			SYSTEMTIME st2 = {0};
			ParseDate(currentAlert.date, &st1);
			ParseDate(previousAlert.date, &st2);
			
			FILETIME ft1 = {0};
			FILETIME ft2 = {0};
			SystemTimeToFileTime(&st1, &ft1);
			SystemTimeToFileTime(&st2, &ft2);
			if (CompareFileTime(&ft1, &ft2) == 1) {
				for (udm i = 0; i < alertsNum; ++i) {
					ParseDate(gAlerts[i].date, &st1);
					SystemTimeToFileTime(&st1, &ft1);
						
					if (!CompareFileTime(&ft1, &ft2)) break;
					ShowAlertNotification(&gAlerts[i]);
				}
			}
				
			currentAlert = previousAlert = gAlerts[0];
		}
		
		Sleep(QUERY_DELAY);
	}
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	
	static u32 WM_TASKBARCREATED;
	static HMENU hMenu;
	
	switch (msg) {
		case WM_CREATE: {
			WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
			gIcon = LoadIconW(GetModuleHandleW(0), MAKEINTRESOURCEW(1));
			gIconDisconnected = LoadIconW(GetModuleHandleW(0), MAKEINTRESOURCEW(2));
			AddTrayIcon(hwnd, gIcon);
			
			hMenu = CreatePopupMenu();
			//AppendMenu(hMenu, MF_STRING, CMD_HISTORY, "Alerts History");
			AppendMenu(hMenu, MF_STRING, CMD_ABOUT, "About");
			AppendMenu(hMenu, MF_STRING, CMD_EXIT, "Exit");
			
			gSession = WinHttpOpen(0, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
								   WINHTTP_NO_PROXY_BYPASS, 0);
			
			CreateThread(0, 0, &AlertsThread, hwnd, 0, 0);
		} break;
		
		case WM_APP_CLICKED: {
			switch (LOWORD(lParam)) {
				case WM_RBUTTONDOWN:
				case WM_CONTEXTMENU: {
					ShowTrayIcon(hwnd, hMenu);
				} break;
			}
		} break;
		
		case WM_APP_RUNNING: {
			ShowTrayMessage(hwnd, NIIF_INFO, APP_NAME" is already running");
		} break;
			
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case CMD_HISTORY: {
					OutputDebugString("NOT IMPLEMENTED!\n");
				} break;
				
				case CMD_ABOUT: {
					MessageBoxW(0, L"Simple utility that sits in system tray, monitors Home Front Command alerts and notifies with Windows toast notifications when they're received.\n\nCopyright (C) 2023 Danny Bog. All rights reserved.", L"CAlert", MB_OK);
				} break;
					
				case CMD_EXIT: {
					DestroyWindow(hwnd);
				} break;
			}
		} break;
			
		case WM_DESTROY: {
			RemoveTrayIcon(hwnd);
			PostQuitMessage(0);
		} break;
			
		default: {
			if (msg == WM_TASKBARCREATED) AddTrayIcon(hwnd, gIcon);
			result = DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	
	return result;
}

#ifdef _DEBUG
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
#else
void __stdcall WinMainCRTStartup() {
#endif
	WNDCLASSW wc = {
		.lpfnWndProc = &WndProc,
		.hInstance = GetModuleHandleW(0),
		.lpszClassName = APP_NAME
	};
	
	HWND existing = FindWindowW(wc.lpszClassName, 0);
	if (existing) {
		PostMessageW(existing, WM_APP_RUNNING, 0, 0);
		ExitProcess(0);
	}
	
	RegisterClassW(&wc);
	HWND hWindow = CreateWindowW(wc.lpszClassName, wc.lpszClassName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
	
	if (hWindow) {
		MSG msg;
		while (GetMessageW(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	
	ExitProcess(0);
}
