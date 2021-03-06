//
// This is a program to save and open file.
// 

#include "stdafx.h"
#include "SimpleTextEditor.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//
HWND hEdit;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnSize(HWND hWnd, int width, int height);

void OnFileOpen(HWND hWnd, HINSTANCE hInst);
void OnFileSaveAs(HWND hWnd, HINSTANCE hInst);
void OnFileExit(HWND hWnd);

// main window
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLETEXTEDITOR, szWindowClass, MAX_LOADSTRING);

    //  Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLETEXTEDITOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SIMPLETEXTEDITOR);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    hInst = hInstance; 

    // Create the main program window.
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    // Display the main program window.
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLETEXTEDITOR));

    MSG msg;

    // Main message loop
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

//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:   OnFileOpen(hWnd, hInst);				break;
		case IDM_SAVEAS: OnFileSaveAs(hWnd, hInst);				break;
		case IDM_EXIT:   OnFileExit(hWnd);						break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
        break;
	case WM_PAINT:   OnPaint(hWnd);									break;
	case WM_CREATE:  OnCreate(hWnd);								break;
	case WM_DESTROY: OnDestroy(hWnd);								break;
	case WM_SIZE:    OnSize(hWnd, LOWORD (lParam), HIWORD (lParam));break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//
void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;

	hDC = BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
}

// create edit control
void OnCreate(HWND hWnd)
{
	hEdit = CreateWindowEx( 0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
		100, 100, 300, 20, hWnd, (HMENU)IDC_EDIT1, hInst, NULL);
}

//
void OnDestroy(HWND hWnd)
{
	PostQuitMessage(0);
}

// resize control with window
void OnSize(HWND hWnd, int width, int height)
{
	MoveWindow(hEdit, 0,  0, width, height, TRUE);
}

//
void OnFileOpen(HWND hWnd, HINSTANCE hInst)
{
	wchar_t szFile[MAX_PATH] = L"";
    OPENFILENAME fn;

	// initialize variable fn to zero
	ZeroMemory(&fn,sizeof(OPENFILENAME));

	// set the variable fn
	fn.lStructSize = sizeof(OPENFILENAME);
	fn.hwndOwner = hWnd;
	fn.hInstance = hInst;
	fn.lpstrFilter = _T("All Files\0*.*\0");
	fn.lpstrFile = szFile;
	fn.nMaxFile = MAX_PATH;
	fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// display open file dialog box
	if (!GetOpenFileName(&fn)) return;

    // add filename to window title
	wchar_t szText[MAX_PATH];
    wcscpy_s(szText,MAX_PATH,szTitle);
    wcscat_s(szText,MAX_PATH,L" - ");
    wcscat_s(szText,MAX_PATH, szFile);
    SetWindowText(hWnd,szText);

	FILE *pf = 0;
	errno_t err;
	int size;
	char* buffer;

	// open file for reading
	if( (err = _wfopen_s( &pf, szFile, L"rb" )) != 0 ) return;

	// read size
	fread(&size, sizeof(int), 1, pf);

	// allocate memory for the text
	buffer = new char[size + 1];

	// read data
	fread(buffer, sizeof(char), size, pf);
	buffer[size] = '\0';

	// close file
	fclose(pf);

	// output text to edit control
	SetWindowTextA(hEdit, buffer);

	// release memory
	delete[] buffer;
}

//
void OnFileSaveAs(HWND hWnd, HINSTANCE hInst)
{
	wchar_t szFile[MAX_PATH] = L"";
    OPENFILENAME fn;

	// initialize variable fn to zero
	ZeroMemory(&fn,sizeof(OPENFILENAME));

	// set the variable fn
	fn.lStructSize = sizeof(OPENFILENAME);
	fn.hwndOwner = hWnd;
	fn.hInstance = hInst;
	fn.lpstrFilter = _T("All Files\0*.*\0");
	fn.lpstrFile = szFile;
	fn.nMaxFile = MAX_PATH;
	fn.Flags =  OFN_OVERWRITEPROMPT;

	// display save as file dialog box
	if(!GetSaveFileName(&fn)) return;

    // add filename to window title
	wchar_t szText[MAX_PATH];
    wcscpy_s(szText,MAX_PATH,szTitle);
    wcscat_s(szText,MAX_PATH,L" - ");
    wcscat_s(szText,MAX_PATH, szFile);
    SetWindowText(hWnd,szText);

	// allocate memory for the text
	int size = GetWindowTextLength(hEdit);
	char* buffer = new char[size + 1];

	// copy the text of the edit control into a buffer
	GetWindowTextA(hEdit, buffer, size);

	FILE *pf = 0;
	errno_t err;

	// open file for writing
	if( (err = _wfopen_s( &pf, szFile, L"wb" )) != 0 ) return;

	// write size
	fwrite(&size, sizeof(int), 1, pf);

	// write data
	fwrite(buffer, sizeof(char), size, pf);

	// close file
	fclose(pf);

	// release memory
	delete[] buffer;
}

//
void OnFileExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}