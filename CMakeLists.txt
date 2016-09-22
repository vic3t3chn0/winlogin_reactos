
add_executable(winlogin winlogin.c)
set_module_type(winlogin win32gui UNICODE)
add_importlibs(winlogin userenv msvcrt kernel32 ntdll)
add_cd_file(TARGET winlogin DESTINATION reactos/system32 FOR all)
