/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#include <windows.h>

#include <main.h>

#pragma once

BOOL WINAPI Mdi(){

 default:
        return DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);

 CLIENTCREATESTRUCT ccs;

    ccs.hWindowMenu  = GetSubMenu(GetMenu(hwnd), 2);
    ccs.idFirstChild = ID_MDI_FIRSTCHILD;

    g_hMDIClient = CreateWindowEx(WS_EX_CLIENTEDGE, "mdiclient", NULL,
        WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwnd, (HMENU)IDC_MAIN_MDI, GetModuleHandle(NULL), (LPVOID)&ccs);

case WM_COMMAND:
        switch(LOWORD(wParam))
        {
            case ID_FILE_EXIT:
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;

            // ... handle other regular IDs ...

            // Handle MDI Window commands
            default:
            {
                if(LOWORD(wParam) >= ID_MDI_FIRSTCHILD)
                {
                    DefFrameProc(hwnd, g_hMDIClient, msg, wParam, lParam);
                }
                else 
                {
                    HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,0);
                    if(hChild)
                    {
                        SendMessage(hChild, WM_COMMAND, wParam, lParam);
                    }
                }
            }
        }
    break;

while(GetMessage(&Msg, NULL, 0, 0))
    {
        if (!TranslateMDISysAccel(g_hMDIClient, &Msg))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
}
BOOL SetUpMDIChildWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MDIChildWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szChildClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(0, "Could Not Register Child Window", "Oh Oh...",
            MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }
    else
        return TRUE;
}


LRESULT CALLBACK MDIChildWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            HFONT hfDefault;
            HWND hEdit;

            // Create Edit Control

            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
                0, 0, 100, 100, hwnd, (HMENU)IDC_CHILD_EDIT, GetModuleHandle(NULL), NULL);
            if(hEdit == NULL)
                MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

            hfDefault = GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
        }
        break;
        case WM_MDIACTIVATE:
        {
            HMENU hMenu, hFileMenu;
            UINT EnableFlag;

            hMenu = GetMenu(g_hMainWindow);
            if(hwnd == (HWND)lParam)
            {      //being activated, enable the menus
                EnableFlag = MF_ENABLED;
            }
            else
            {                          //being de-activated, gray the menus
                EnableFlag = MF_GRAYED;
            }

            EnableMenuItem(hMenu, 1, MF_BYPOSITION | EnableFlag);
            EnableMenuItem(hMenu, 2, MF_BYPOSITION | EnableFlag);

            hFileMenu = GetSubMenu(hMenu, 0);
            EnableMenuItem(hFileMenu, ID_FILE_SAVEAS, MF_BYCOMMAND | EnableFlag);

            EnableMenuItem(hFileMenu, ID_FILE_CLOSE, MF_BYCOMMAND | EnableFlag);
            EnableMenuItem(hFileMenu, ID_FILE_CLOSEALL, MF_BYCOMMAND | EnableFlag);

            DrawMenuBar(g_hMainWindow);
        }
        break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_OPEN:
                    DoFileOpen(hwnd);
                break;
                case ID_FILE_SAVEAS:
                    DoFileSave(hwnd);
                break;
                case ID_EDIT_CUT:
                    SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_CUT, 0, 0);
                break;
                case ID_EDIT_COPY:
                    SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_COPY, 0, 0);
                break;
                case ID_EDIT_PASTE:
                    SendDlgItemMessage(hwnd, IDC_CHILD_EDIT, WM_PASTE, 0, 0);
                break;
            }
        break;
        case WM_SIZE:
        {
            HWND hEdit;
            RECT rcClient;

            // Calculate remaining height and size edit

            GetClientRect(hwnd, &rcClient);

            hEdit = GetDlgItem(hwnd, IDC_CHILD_EDIT);
            SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
        }
        return DefMDIChildProc(hwnd, msg, wParam, lParam);
        default:
            return DefMDIChildProc(hwnd, msg, wParam, lParam);
    
    }
    return 0;
}


HWND CreateNewMDIChild(HWND hMDIClient)
{ 
    MDICREATESTRUCT mcs;
    HWND hChild;

    mcs.szTitle = "[Untitled]";
    mcs.szClass = g_szChildClassName;
    mcs.hOwner  = GetModuleHandle(NULL);
    mcs.x = mcs.cx = CW_USEDEFAULT;
    mcs.y = mcs.cy = CW_USEDEFAULT;
    mcs.style = MDIS_ALLCHILDSTYLES;

    hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
    if(!hChild)
    {
        MessageBox(hMDIClient, "MDI Child creation failed.", "Oh Oh...",
            MB_ICONEXCLAMATION | MB_OK);
    }

    case WM_CREATE:
    {
        CREATESTRUCT* pCreateStruct;
        MDICREATESTRUCT* pMDICreateStruct;

        pCreateStruct = (CREATESTRUCT*)lParam;
        pMDICreateStruct = (MDICREATESTRUCT*)pCreateStruct->lpCreateParams;

        /*
        pMDICreateStruct now points to the same MDICREATESTRUCT that you
        sent along with the WM_MDICREATE message and you can use it
        to access the lParam.
        */
    }
    break;

    case ID_FILE_NEW:
        CreateNewMDIChild(g_hMDIClient);
    break;
    case ID_FILE_OPEN:
    {
        HWND hChild = CreateNewMDIChild(g_hMDIClient);
        if(hChild)
        {
            DoFileOpen(hChild); 
        }
    }
    break;
    case ID_FILE_CLOSE:
    {
        HWND hChild = (HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,0);
        if(hChild)
        {
            SendMessage(hChild, WM_CLOSE, 0, 0);
        }
    }
    break;

     case ID_WINDOW_TILE:
        SendMessage(g_hMDIClient, WM_MDITILE, 0, 0);
    break;
    case ID_WINDOW_CASCADE:
        SendMessage(g_hMDIClient, WM_MDICASCADE, 0, 0);
    break;
    
    return hChild;
}

