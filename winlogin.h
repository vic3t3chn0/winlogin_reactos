#ifndef _WINLOGIN_H
#define _WINLOGIN_H

#ifndef STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE
#endif

#include <stdarg.h>

#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <winnls.h>
#include <wingdi.h>
#include <shellapi.h>
#include <commdlg.h>
#include <tchar.h>
#include <malloc.h>

#include "main.h"

void UpdateWindowCaption(BOOL clearModifyAlert);

#endif /* _WINLOGIN_H */
