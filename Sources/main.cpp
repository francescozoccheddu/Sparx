#pragma once

#include <Windows.h>
#include <Resources.hpp>
#include <ErrorLogger.hpp>

constexpr int c_initialWidth { 640 };
constexpr int c_initialHeight { 480 };

constexpr int c_minWidth { 320 };
constexpr int c_minHeight { 240 };

constexpr bool c_handleSizingOnDragResizing { true };

LRESULT CALLBACK MainWinProc (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int APIENTRY wWinMain (_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	UNREFERENCED_PARAMETER (_hPrevInstance);
	UNREFERENCED_PARAMETER (_lpCmdLine);

	LPCTSTR title;
	if (LoadString (_hInstance, IDS_TITLE, (LPTSTR) &title, 0) <= 0)
	{
		Sparx::Error::Post ("Unable to load title resource");
	}

	WNDCLASSEX mainClass;
	mainClass.style = CS_VREDRAW | CS_HREDRAW;
	mainClass.cbSize = sizeof (WNDCLASSEX);
	mainClass.lpfnWndProc = &MainWinProc;
	mainClass.cbClsExtra = 0;
	mainClass.cbWndExtra = 0;
	mainClass.hInstance = _hInstance;
	mainClass.hIcon = LoadIcon (_hInstance, MAKEINTRESOURCE (IDI_NORMAL));
	mainClass.hIconSm = LoadIcon (_hInstance, MAKEINTRESOURCE (IDI_SMALL));
	mainClass.hCursor = LoadCursor (NULL, IDC_ARROW);
	mainClass.hbrBackground = GetSysColorBrush (COLOR_WINDOW);
	mainClass.lpszMenuName = nullptr;
	mainClass.lpszClassName = TEXT ("Main");

	ATOM classAtom = RegisterClassEx (&mainClass);
	if (!classAtom)
	{
		Sparx::Error::Post ("Unable to register window class");
	}

	DWORD style { WS_OVERLAPPEDWINDOW | WS_VISIBLE };
	HWND hWnd = CreateWindow (MAKEINTATOM (classAtom), title, style, CW_USEDEFAULT, CW_USEDEFAULT, c_initialWidth, c_initialHeight, NULL, NULL, _hInstance, NULL);

	if (!hWnd)
	{
		Sparx::Error::Post ("Unable to create window");
	}
	else
	{
		ShowWindow (hWnd, _nCmdShow);
		UpdateWindow (hWnd);

		MSG msg = {};
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
			else
			{
				//PGAME_DO (Tick ());
			}
		}
	}

	Sparx::Error::PreQuit ();

	return 0;
}

LRESULT CALLBACK MainWinProc (HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	static bool s_bSizing = false;
	static bool s_bSuspended = false;
	static bool s_bMinimized = false;

	switch (_msg)
	{
		case WM_CREATE:
		{
			RECT rc;
			GetClientRect (_hWnd, &rc);
			//PGAME_DO (SetWindow (_hWnd, { rc.right - rc.left, rc.bottom - rc.top }, WindowRotation::IDENTITY));
		}
		break;

		case WM_CLOSE:
		{
			DestroyWindow (_hWnd);
		}
		break;

		case WM_PAINT:
		{
			if (s_bSizing)
			{
				//PGAME_DO (Tick ());
			}
			else
			{
				hdc = BeginPaint (_hWnd, &ps);
				EndPaint (_hWnd, &ps);
			}
		}
		break;

		case WM_SIZE:
		{
			if (_wParam == SIZE_MINIMIZED)
			{
				if (!s_bMinimized)
				{
					s_bMinimized = true;
					if (!s_bSuspended)
					{
						//PGAME_DO (Suspend ());
					}
					s_bSuspended = true;
				}
			}
			else if (s_bMinimized)
			{
				s_bMinimized = false;
				if (s_bSuspended)
				{
					//PGAME_DO (Resume ());
					Sparx::Error::Log ("Ciao");
				}
				s_bSuspended = false;
			}
			else
			{
#if !c_handleSizingOnDragResizing
				if (!s_bSizing)
#endif
				{
					//PGAME_DO (Size ({ LOWORD (_lParam), HIWORD (_lParam) }, WindowRotation::IDENTITY));
				}
			}
		}
		break;

		case WM_ENTERSIZEMOVE:
		{
			s_bSizing = true;
		}
		break;

		case WM_EXITSIZEMOVE:
		{
			s_bSizing = false;
			if (c_handleSizingOnDragResizing)
			{
				RECT rc;
				GetClientRect (_hWnd, &rc);
				//GAME_DO (Size ({ rc.right - rc.left, rc.bottom - rc.top }));
			}
		}
		break;

		case WM_GETMINMAXINFO:
		{
			MINMAXINFO* pInfo = reinterpret_cast<MINMAXINFO*>(_lParam);
			pInfo->ptMinTrackSize.x = c_minWidth;
			pInfo->ptMinTrackSize.y = c_minHeight;
		}
		break;

		case WM_POWERBROADCAST:
		{
			switch (_wParam)
			{
				case PBT_APMQUERYSUSPEND:
					if (!s_bSuspended)
					{
						//PGAME_DO (Suspend ());
					}
					s_bSuspended = true;
					return TRUE;

				case PBT_APMRESUMESUSPEND:
					if (!s_bMinimized)
					{
						if (s_bSuspended)
						{
							//PGAME_DO (Resume ());
						}
						s_bSuspended = false;
					}
					return TRUE;
			}
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage (0);
		}
		break;
	}

	return DefWindowProc (_hWnd, _msg, _wParam, _lParam);
}