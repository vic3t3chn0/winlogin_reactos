<?xml version="1.0"?>
<!DOCTYPE module SYSTEM "../../../tools/rbuild/project.dtd">
<module name="WinLogin" type="win32gui" installbase="system32" installname="winlogin.exe" unicode="yes">
   <bootstrap installbase="$(CDOUTPUT)" />
   <include base="WinLogin">.</include>
   <library>kernel32</library>
   <library>gdi32</library>
   <library>user32</library>
   <file>winlogin.c</file>
   <file>settings.c</file>
   <file>process.c</file>
   <file>text.c</file>
   <file>user.c</file>
</module>
