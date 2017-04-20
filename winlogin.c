#define STRICT

#include <windows.h>

#include <winlogin.h>


// Register the window class.
const wchart_t g_szClassName[]  = "WinLogin Class";
    
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WINLOGIN_GLOBALS Globals;


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



/*
 *  OnSize
 *      If we have an inner child, resize it to fit.
 */
/**void
OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    if (g_hwndChild) {
        MoveWindow(g_hwndChild, 0, 0, cx, cy, TRUE);
    }
}*/

/*
 *  OnCreate
 *      Applications will typically override this and maybe even
 *      create a child window.
 */
/**BOOL
OnCreate(HWND hwnd, LPCREATESTRUCT lpcs)
{
    return TRUE;
}*/

/*
 *  OnDestroy
 *      Post a quit message because our application is over when the
 *      user closes this window.
 */
/**void
OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}*/



/***********************************************************************
 *
 *           WinMain
 */
 int WINAPI WinMain(HINSTANCE hInstance, 
                HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    WINLOGIN_LoadSettingsFromRegistry(void);
    WINLOGIN_SaveSettingsToRegistry(void);

   
    wc.cbsize   = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szClassName;

    if(!RegisterClassEx(&wc)){
        return 0;
    }

    switch (GetUserDefaultUILanguage())
    {
    case MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT):
        SetProcessDefaultLayout(LAYOUT_RTL);
        break;

    default:
        break;
    }

    WINLOGIN_InitData();

    
    // Create the window.

   hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "WinLogin",                     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style
    CW_USEDEFAULT, CW_USEDEFAULT. 240, 120, NULL, NULL, hInstance, NULL);  // Size and position


    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);



    // Run the message loop.
    while(GetMessage(&Msg, NULL, 0,0) > 0){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    

    return Msg.wParam;

}
