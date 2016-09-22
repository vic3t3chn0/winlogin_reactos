
list(APPEND SOURCE
    winlogin.c
    text.c
    main.c
    settings.c
    main.h
    winlogin.h)

add_executable(winlogin ${SOURCE} winlogin.c)
set_module_type(winlogin win32gui UNICODE)
add_importlibs(winlogin userenv msvcrt kernel32 ntdll)
add_cd_file(TARGET winlogin DESTINATION reactos/system32 FOR all)
