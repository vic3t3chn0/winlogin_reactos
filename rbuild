<?xml version="1.0"?>
<!DOCTYPE module SYSTEM "../../../tools/rbuild/project.dtd">
<module name="winlogin" type="win32gui" installbase="system32" installname="winlogin.exe" unicode="yes">
   <bootstrap installbase="$(CDOUTPUT)" />
   <include base="winlogin">.</include>
   <library>kernel32</library>
   <library>gdi32</library>
   <library>user32</library>
   <file>winlogin.c</file>
</module>