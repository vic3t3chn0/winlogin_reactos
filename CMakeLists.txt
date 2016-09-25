
#include_directories(
#    ${REACTOS_SOURCE_DIR}/include/reactos/idl
#    ${CMAKE_CURRENT_BINARY_DIR})

#add_rpc_files(server ${REACTOS_SOURCE_DIR}/include/reactos/idl/winreg.idl)

list(APPEND SOURCE
     user.c
     process.c
     settings.c
     text.c
     winlogin.c
     winlogin.h
     process.h)

#add_executable(winlogin ${SOURCE} winlogin.rc)
#target_link_libraries(winlogin wine)
set_module_type(winlogin win32gui UNICODE)
add_importlibs(winlogin user32 gdi32 comctl32 comdlg32 advapi32 shell32 msvcrt kernel32)
add_pch(winlogin winlogin.h SOURCE)
#add_cd_file(TARGET winlogin DESTINATION reactos/winlogin FOR all)
