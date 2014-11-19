# Microsoft Developer Studio Project File - Name="ac3decgui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ac3decgui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ac3decgui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ac3decgui.mak" CFG="ac3decgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ac3decgui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ac3decgui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ac3decgui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ac3decgui___Win32_Release"
# PROP BASE Intermediate_Dir "ac3decgui___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ac3decgui___Win32_Release"
# PROP Intermediate_Dir "ac3decgui___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /GB /Zp4 /W3 /GX /Ox /Op /Ob2 /I "aac" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 mbaacenc.lib winmm.lib msacm32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /out:"ac3decgui.exe" /libpath:"aac"

!ELSEIF  "$(CFG)" == "ac3decgui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ac3decgui___Win32_Debug"
# PROP BASE Intermediate_Dir "ac3decgui___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ac3decgui___Win32_Debug"
# PROP Intermediate_Dir "ac3decgui___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gm /GX /Zi /Od /I "aac" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mbaacenc.lib winmm.lib msacm32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /out:"ac3decgui_debug.exe" /libpath:"aac"

!ENDIF 

# Begin Target

# Name "ac3decgui - Win32 Release"
# Name "ac3decgui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "output"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\output_aacfile.c
# End Source File
# Begin Source File

SOURCE=.\output_wavefile.c
# End Source File
# Begin Source File

SOURCE=.\output_winwave.c
# End Source File
# Begin Source File

SOURCE=.\ring_buffer.c
# End Source File
# End Group
# Begin Group "vob_demux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Bbdbits.cpp
# End Source File
# Begin Source File

SOURCE=.\Bbdmux.cpp
# End Source File
# Begin Source File

SOURCE=.\demuxbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\my_fread.cpp
# End Source File
# End Group
# Begin Group "gui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\guimain.c

!IF  "$(CFG)" == "ac3decgui - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "ac3decgui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "ac3dec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bit_allocate.c
# End Source File
# Begin Source File

SOURCE=.\bitstream.c
# End Source File
# Begin Source File

SOURCE=.\crc.c
# End Source File
# Begin Source File

SOURCE=.\debug.c
# End Source File
# Begin Source File

SOURCE=.\dither.c
# End Source File
# Begin Source File

SOURCE=.\Downmix.c
# End Source File
# Begin Source File

SOURCE=.\exponent.c
# End Source File
# Begin Source File

SOURCE=.\imdct.c
# End Source File
# Begin Source File

SOURCE=.\main.c

!IF  "$(CFG)" == "ac3decgui - Win32 Release"

# ADD CPP /O2 /Ob0

!ELSEIF  "$(CFG)" == "ac3decgui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mantissa.c
# End Source File
# Begin Source File

SOURCE=.\parse.c
# End Source File
# Begin Source File

SOURCE=.\rematrix.c
# End Source File
# Begin Source File

SOURCE=.\stats.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ac3dec.ico
# End Source File
# Begin Source File

SOURCE=.\dollar.bmp
# End Source File
# Begin Source File

SOURCE=.\guimain.rc
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# End Target
# End Project
