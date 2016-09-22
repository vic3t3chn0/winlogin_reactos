#include "winlogin.h"

#include <strsafe.h>

WINLOGIN_GLOBALS Globals;
static ATOM aFINDMSGSTRING;

VOID WINLOGIN_EnableSearchMenu()
{
    EnableMenuItem(Globals.hMenu, CMD_SEARCH,
                   MF_BYCOMMAND | ((GetWindowTextLength(Globals.hEdit) == 0) ? MF_DISABLED | MF_GRAYED : MF_ENABLED));
    EnableMenuItem(Globals.hMenu, CMD_SEARCH_NEXT,
                   MF_BYCOMMAND | ((GetWindowTextLength(Globals.hEdit) == 0) ? MF_DISABLED | MF_GRAYED : MF_ENABLED));
}

/***********************************************************************
 *
 *           NOTEPAD_MenuCommand
 *
 *  All handling of main menu events
 */
static int NOTEPAD_MenuCommand(WPARAM wParam)
{
    switch (wParam)
    {
    case CMD_NEW:        DIALOG_FileNew(); break;
    case CMD_OPEN:       DIALOG_FileOpen(); break;
    case CMD_SAVE:       DIALOG_FileSave(); break;
    case CMD_SAVE_AS:    DIALOG_FileSaveAs(); break;
    case CMD_PRINT:      DIALOG_FilePrint(); break;
    case CMD_PAGE_SETUP: DIALOG_FilePageSetup(); break;
    case CMD_EXIT:       DIALOG_FileExit(); break;

    case CMD_UNDO:       DIALOG_EditUndo(); break;
    case CMD_CUT:        DIALOG_EditCut(); break;
    case CMD_COPY:       DIALOG_EditCopy(); break;
    case CMD_PASTE:      DIALOG_EditPaste(); break;
    case CMD_DELETE:     DIALOG_EditDelete(); break;
    case CMD_SELECT_ALL: DIALOG_EditSelectAll(); break;
    case CMD_TIME_DATE:  DIALOG_EditTimeDate(); break;

    case CMD_SEARCH:      DIALOG_Search(); break;
    case CMD_SEARCH_NEXT: DIALOG_SearchNext(); break;
    case CMD_REPLACE:     DIALOG_Replace(); break;
    case CMD_GOTO:        DIALOG_GoTo(); break;

    case CMD_WRAP: DIALOG_EditWrap(); break;
    case CMD_FONT: DIALOG_SelectFont(); break;

    case CMD_STATUSBAR: DIALOG_ViewStatusBar(); break;

    case CMD_HELP_CONTENTS: DIALOG_HelpContents(); break;

    case CMD_ABOUT:
        DialogBox(GetModuleHandle(NULL),
                  MAKEINTRESOURCE(IDD_ABOUTBOX),
                  Globals.hMainWnd,
                  AboutDialogProc);
        break;

    case CMD_HELP_ABOUT_NOTEPAD: DIALOG_HelpAboutNotepad(); break;

    default:
        break;
    }
    return 0;
}


/***********************************************************************
 *
 *           NOTEPAD_FindTerm
 */

static VOID NOTEPAD_FindTerm(VOID)
{
    Globals.hFindReplaceDlg = NULL;
}

/***********************************************************************
 * Data Initialization
 */
static VOID WINLOGIN_InitData(VOID)
{
    LPTSTR p = Globals.szFilter;
    static const TCHAR txt_files[] = _T("*.txt");
    static const TCHAR all_files[] = _T("*.*");

    p += LoadString(Globals.hInstance, STRING_TEXT_FILES_TXT, p, MAX_STRING_LEN) + 1;
    _tcscpy(p, txt_files);
    p += ARRAY_SIZE(txt_files);

    p += LoadString(Globals.hInstance, STRING_ALL_FILES, p, MAX_STRING_LEN) + 1;
    _tcscpy(p, all_files);
    p += ARRAY_SIZE(all_files);
    *p = '\0';
    Globals.find.lpstrFindWhat = NULL;

    Globals.hDevMode = NULL;
    Globals.hDevNames = NULL;
}

/***********************************************************************
 * Enable/disable items on the menu based on control state
 */
static VOID NOTEPAD_InitMenuPopup(HMENU menu, LPARAM index)
{
    int enable;

    UNREFERENCED_PARAMETER(index);

    CheckMenuItem(GetMenu(Globals.hMainWnd), CMD_WRAP,
        MF_BYCOMMAND | (Globals.bWrapLongLines ? MF_CHECKED : MF_UNCHECKED));
    if (!Globals.bWrapLongLines)
    {
        CheckMenuItem(GetMenu(Globals.hMainWnd), CMD_STATUSBAR,
            MF_BYCOMMAND | (Globals.bShowStatusBar ? MF_CHECKED : MF_UNCHECKED));
    }
    EnableMenuItem(menu, CMD_UNDO,
        SendMessage(Globals.hEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(menu, CMD_PASTE,
        IsClipboardFormatAvailable(CF_TEXT) ? MF_ENABLED : MF_GRAYED);
    enable = (int) SendMessage(Globals.hEdit, EM_GETSEL, 0, 0);
    enable = (HIWORD(enable) == LOWORD(enable)) ? MF_GRAYED : MF_ENABLED;
    EnableMenuItem(menu, CMD_CUT, enable);
    EnableMenuItem(menu, CMD_COPY, enable);
    EnableMenuItem(menu, CMD_DELETE, enable);

    EnableMenuItem(menu, CMD_SELECT_ALL,
        GetWindowTextLength(Globals.hEdit) ? MF_ENABLED : MF_GRAYED);
    DrawMenuBar(Globals.hMainWnd);
}


/***********************************************************************
 *
 *           NOTEPAD_WndProc
 */

static int AlertFileDoesNotExist(LPCTSTR szFileName)
{
    return DIALOG_StringMsgBox(Globals.hMainWnd, STRING_DOESNOTEXIST,
                               szFileName,
                               MB_ICONEXCLAMATION | MB_YESNO);
}

static BOOL HandleCommandLine(LPTSTR cmdline)
{
    BOOL opt_print = FALSE;

    while (*cmdline == _T(' ') || *cmdline == _T('-') || *cmdline == _T('/'))
    {
        TCHAR option;

        if (*cmdline++ == _T(' ')) continue;

        option = *cmdline;
        if (option) cmdline++;
        while (*cmdline == _T(' ')) cmdline++;

        switch(option)
        {
            case 'p':
            case 'P':
                opt_print = TRUE;
                break;
        }
    }

    if (*cmdline)
    {
        /* file name is passed in the command line */
        LPCTSTR file_name = NULL;
        BOOL file_exists = FALSE;
        TCHAR buf[MAX_PATH];

        if (cmdline[0] == _T('"'))
        {
            cmdline++;
            cmdline[lstrlen(cmdline) - 1] = 0;
        }

        file_name = cmdline;
        if (FileExists(file_name))
        {
            file_exists = TRUE;
        }
        else if (!HasFileExtension(cmdline))
        {
            static const TCHAR txt[] = _T(".txt");

            /* try to find file with ".txt" extension */
            if (!_tcscmp(txt, cmdline + _tcslen(cmdline) - _tcslen(txt)))
            {
                file_exists = FALSE;
            }
            else
            {
                _tcsncpy(buf, cmdline, MAX_PATH - _tcslen(txt) - 1);
                _tcscat(buf, txt);
                file_name = buf;
                file_exists = FileExists(file_name);
            }
        }

        if (file_exists)
        {
            DoOpenFile(file_name);
            InvalidateRect(Globals.hMainWnd, NULL, FALSE);
            if (opt_print)
            {
                DIALOG_FilePrint();
                return FALSE;
            }
        }
        else
        {
            switch (AlertFileDoesNotExist(file_name)) {
            case IDYES:
                DoOpenFile(file_name);
                break;

            case IDNO:
                break;
            }
        }
    }

    return TRUE;
}

/***********************************************************************
 *
 *           WinMain
 */
 int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev,
                   LPSTR lpCmdLine, int nShowCmd)
{
    MSG msg;
    HACCEL hAccel;
    WNDCLASSEX wndclass;
    HMONITOR monitor;
    MONITORINFO info;
    INT x, y;
    HWND hwnd;

    static const TCHAR className[] = _T("Notepad");
    static const TCHAR winName[] = _T("Notepad");


    switch (GetUserDefaultUILanguage())
    {
    case MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT):
        SetProcessDefaultLayout(LAYOUT_RTL);
        break;

    default:
        break;
    }

    UNREFERENCED_PARAMETER(prev);

    aFINDMSGSTRING = (ATOM)RegisterWindowMessage(FINDMSGSTRING);

    ZeroMemory(&Globals, sizeof(Globals));
    Globals.hInstance = hInstance;
    WINLOGIN_LoadSettingsFromRegistry();

    g_hinst = hinst;
    DWORD *pMessage;

        if (!RegisterClassEx(&wndclass)) return FALSE;

    /* Setup windows */

    monitor = MonitorFromRect(&Globals.main_rect, MONITOR_DEFAULTTOPRIMARY);
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);

    x = Globals.main_rect.left;
    y = Globals.main_rect.top;
    if (Globals.main_rect.left >= info.rcWork.right ||
        Globals.main_rect.top >= info.rcWork.bottom ||
        Globals.main_rect.right < info.rcWork.left ||
        Globals.main_rect.bottom < info.rcWork.top)
        x = y = CW_USEDEFAULT;



    if (!InitApp()) return 0;

    if (SUCCEEDED(CoInitialize(NULL))) {/* In case we use COM */

        hwnd = CreateWindow(
            TEXT("Scratch"),                /* Class Name */
            TEXT("Scratch"),                /* Title */
            WS_OVERLAPPEDWINDOW,            /* Style */
            CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
            CW_USEDEFAULT, CW_USEDEFAULT,   /* Size */
            NULL,                           /* Parent */
            NULL,                           /* No menu */
            hinst,                          /* Instance */
            0);                             /* No special parameters */

        ShowWindow(hwnd, nShowCmd);

        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        CoUninitialize();
    }

    DoCreateEditWindow();

    WINLOGIN_InitData();
   
    ShowWindow(Globals.hMainWnd, show);
    UpdateWindow(Globals.hMainWnd);
    DragAcceptFiles(Globals.hMainWnd, TRUE);

    DIALOG_ViewStatusBar();

    if (!HandleCommandLine(cmdline))
    {
        return 0;
    }

    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(ID_ACCEL));



    //-- wait for the process to finish
while (true)
{
  //-- see if the task has terminated
  DWORD dwExitCode = WaitForSingleObject(ProcessInfo.hProcess, 0);

  if (   (dwExitCode == WAIT_FAILED   )
      || (dwExitCode == WAIT_OBJECT_0 )
      || (dwExitCode == WAIT_ABANDONED) )
  {
    DWORD dwExitCode;

    //-- get the process exit code
    GetExitCodeProcess(ProcessInfo.hProcess, &dwExitCode);

    //-- the task has ended so close the handle
    CloseHandle(ProcessInfo.hThread);
    CloseHandle(ProcessInfo.hProcess);

    //-- save the exit code
    lExitCode = dwExitCode;

    return;
  }
  else
  {
    //-- see if there are any message that need to be processed
    while (PeekMessage(&message.msg, 0, 0, 0, PM_NOREMOVE))
    {
      if (message.msg.message == WM_QUIT)
      {
        return;
      }

      //-- process the message queue
      if (GetMessage(&message.msg, 0, 0, 0))
      {
        //-- process the message
        TranslateMessage(&Message->msg);
        DispatchMessage(&pMessage->msg);
      }
    }
  }

    return 0;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE prev, LPTSTR cmdline, int show)
{

    if (!Globals.hMainWnd)
    {
        ShowLastError();
        ExitProcess(1);
    }

    
    while (GetMessage(&msg, 0, 0, 0))
    {
        if (!IsDialogMessage(Globals.hFindReplaceDlg, &msg) &&
            !TranslateAccelerator(Globals.hMainWnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}
