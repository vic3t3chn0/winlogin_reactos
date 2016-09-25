
<<<<<<< HEAD
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
=======
list(APPEND SOURCE
    drivepage.c
    reactos.c
    reactos.h)

file(GLOB reactos_rc_deps res/*.*)
add_rc_deps(reactos.rc ${reactos_rc_deps})
add_executable(reactos ${SOURCE} reactos.rc)
add_pch(reactos reactos.h SOURCE)
target_link_libraries(reactos uuid)
set_module_type(reactos win32gui UNICODE)
add_importlibs(reactos advapi32 gdi32 user32 comctl32 setupapi msvcrt kernel32 ntdll)
add_cd_file(TARGET reactos DESTINATION reactos NO_CAB FOR bootcd)

add_subdirectory(setup)
>>>>>>> 08d1779325939c26486f49821fa8618496e0ad9f
