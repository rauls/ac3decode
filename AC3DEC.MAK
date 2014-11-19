# Microsoft Developer Studio Generated NMAKE File, Based on ac3dec.dsp
!IF "$(CFG)" == ""
CFG=ac3dec - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ac3dec - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ac3dec - Win32 Release" && "$(CFG)" != "ac3dec - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ac3dec.mak" CFG="ac3dec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ac3dec - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ac3dec - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ac3dec - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ac3dec.exe" "$(OUTDIR)\ac3dec.bsc"


CLEAN :
	-@erase "$(INTDIR)\Bbdbits.obj"
	-@erase "$(INTDIR)\Bbdbits.sbr"
	-@erase "$(INTDIR)\Bbdmux.obj"
	-@erase "$(INTDIR)\Bbdmux.sbr"
	-@erase "$(INTDIR)\bit_allocate.obj"
	-@erase "$(INTDIR)\bit_allocate.sbr"
	-@erase "$(INTDIR)\bitstream.obj"
	-@erase "$(INTDIR)\bitstream.sbr"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\crc.sbr"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\debug.sbr"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\decode.sbr"
	-@erase "$(INTDIR)\demuxbuf.obj"
	-@erase "$(INTDIR)\demuxbuf.sbr"
	-@erase "$(INTDIR)\dither.obj"
	-@erase "$(INTDIR)\dither.sbr"
	-@erase "$(INTDIR)\downmix.obj"
	-@erase "$(INTDIR)\downmix.sbr"
	-@erase "$(INTDIR)\exponent.obj"
	-@erase "$(INTDIR)\exponent.sbr"
	-@erase "$(INTDIR)\imdct.obj"
	-@erase "$(INTDIR)\imdct.sbr"
	-@erase "$(INTDIR)\mantissa.obj"
	-@erase "$(INTDIR)\mantissa.sbr"
	-@erase "$(INTDIR)\my_fread.obj"
	-@erase "$(INTDIR)\my_fread.sbr"
	-@erase "$(INTDIR)\output_aacfile.obj"
	-@erase "$(INTDIR)\output_aacfile.sbr"
	-@erase "$(INTDIR)\output_wavefile.obj"
	-@erase "$(INTDIR)\output_wavefile.sbr"
	-@erase "$(INTDIR)\output_winwave.obj"
	-@erase "$(INTDIR)\output_winwave.sbr"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\parse.sbr"
	-@erase "$(INTDIR)\rematrix.obj"
	-@erase "$(INTDIR)\rematrix.sbr"
	-@erase "$(INTDIR)\ring_buffer.obj"
	-@erase "$(INTDIR)\ring_buffer.sbr"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\stats.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ac3dec.bsc"
	-@erase "$(OUTDIR)\ac3dec.exe"
	-@erase "$(OUTDIR)\ac3dec.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /Zp4 /ML /W3 /GR /GX /Z7 /O2 /Op /I "aac" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\ac3dec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ac3dec.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\bit_allocate.sbr" \
	"$(INTDIR)\bitstream.sbr" \
	"$(INTDIR)\crc.sbr" \
	"$(INTDIR)\debug.sbr" \
	"$(INTDIR)\decode.sbr" \
	"$(INTDIR)\dither.sbr" \
	"$(INTDIR)\downmix.sbr" \
	"$(INTDIR)\exponent.sbr" \
	"$(INTDIR)\imdct.sbr" \
	"$(INTDIR)\mantissa.sbr" \
	"$(INTDIR)\parse.sbr" \
	"$(INTDIR)\rematrix.sbr" \
	"$(INTDIR)\stats.sbr" \
	"$(INTDIR)\Bbdbits.sbr" \
	"$(INTDIR)\Bbdmux.sbr" \
	"$(INTDIR)\demuxbuf.sbr" \
	"$(INTDIR)\my_fread.sbr" \
	"$(INTDIR)\output_aacfile.sbr" \
	"$(INTDIR)\output_wavefile.sbr" \
	"$(INTDIR)\output_winwave.sbr" \
	"$(INTDIR)\ring_buffer.sbr"

"$(OUTDIR)\ac3dec.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib winmm.lib user32.lib gdi32.lib msacm32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\ac3dec.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ac3dec.exe" /libpath:"aac/" 
LINK32_OBJS= \
	"$(INTDIR)\bit_allocate.obj" \
	"$(INTDIR)\bitstream.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\dither.obj" \
	"$(INTDIR)\downmix.obj" \
	"$(INTDIR)\exponent.obj" \
	"$(INTDIR)\imdct.obj" \
	"$(INTDIR)\mantissa.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\rematrix.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\Bbdbits.obj" \
	"$(INTDIR)\Bbdmux.obj" \
	"$(INTDIR)\demuxbuf.obj" \
	"$(INTDIR)\my_fread.obj" \
	"$(INTDIR)\output_aacfile.obj" \
	"$(INTDIR)\output_wavefile.obj" \
	"$(INTDIR)\output_winwave.obj" \
	"$(INTDIR)\ring_buffer.obj"

"$(OUTDIR)\ac3dec.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ac3dec.exe"


CLEAN :
	-@erase "$(INTDIR)\Bbdbits.obj"
	-@erase "$(INTDIR)\Bbdmux.obj"
	-@erase "$(INTDIR)\bit_allocate.obj"
	-@erase "$(INTDIR)\bitstream.obj"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\demuxbuf.obj"
	-@erase "$(INTDIR)\dither.obj"
	-@erase "$(INTDIR)\downmix.obj"
	-@erase "$(INTDIR)\exponent.obj"
	-@erase "$(INTDIR)\imdct.obj"
	-@erase "$(INTDIR)\mantissa.obj"
	-@erase "$(INTDIR)\my_fread.obj"
	-@erase "$(INTDIR)\output_aacfile.obj"
	-@erase "$(INTDIR)\output_wavefile.obj"
	-@erase "$(INTDIR)\output_winwave.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\rematrix.obj"
	-@erase "$(INTDIR)\ring_buffer.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ac3dec.exe"
	-@erase "$(OUTDIR)\ac3dec.ilk"
	-@erase "$(OUTDIR)\ac3dec.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /Zp4 /MLd /W3 /Gm /GX /ZI /Od /I "aac" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\ac3dec.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ac3dec.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib winmm.lib user32.lib gdi32.lib msacm32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib mbaacenc.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\ac3dec.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ac3dec.exe" /pdbtype:sept /libpath:"aac" 
LINK32_OBJS= \
	"$(INTDIR)\bit_allocate.obj" \
	"$(INTDIR)\bitstream.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\dither.obj" \
	"$(INTDIR)\downmix.obj" \
	"$(INTDIR)\exponent.obj" \
	"$(INTDIR)\imdct.obj" \
	"$(INTDIR)\mantissa.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\rematrix.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\Bbdbits.obj" \
	"$(INTDIR)\Bbdmux.obj" \
	"$(INTDIR)\demuxbuf.obj" \
	"$(INTDIR)\my_fread.obj" \
	"$(INTDIR)\output_aacfile.obj" \
	"$(INTDIR)\output_wavefile.obj" \
	"$(INTDIR)\output_winwave.obj" \
	"$(INTDIR)\ring_buffer.obj"

"$(OUTDIR)\ac3dec.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ac3dec.dep")
!INCLUDE "ac3dec.dep"
!ELSE 
!MESSAGE Warning: cannot find "ac3dec.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ac3dec - Win32 Release" || "$(CFG)" == "ac3dec - Win32 Debug"
SOURCE=.\bit_allocate.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\bit_allocate.obj"	"$(INTDIR)\bit_allocate.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\bit_allocate.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bitstream.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\bitstream.obj"	"$(INTDIR)\bitstream.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\bitstream.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\crc.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\crc.obj"	"$(INTDIR)\crc.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\crc.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\debug.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\debug.obj"	"$(INTDIR)\debug.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\decode.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\decode.obj"	"$(INTDIR)\decode.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\decode.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dither.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\dither.obj"	"$(INTDIR)\dither.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\dither.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\downmix.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\downmix.obj"	"$(INTDIR)\downmix.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\downmix.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\exponent.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\exponent.obj"	"$(INTDIR)\exponent.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\exponent.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\imdct.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\imdct.obj"	"$(INTDIR)\imdct.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\imdct.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mantissa.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\mantissa.obj"	"$(INTDIR)\mantissa.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\mantissa.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\parse.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\parse.obj"	"$(INTDIR)\parse.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\parse.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rematrix.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\rematrix.obj"	"$(INTDIR)\rematrix.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\rematrix.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stats.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\stats.obj"	"$(INTDIR)\stats.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\stats.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Bbdbits.cpp

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\Bbdbits.obj"	"$(INTDIR)\Bbdbits.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\Bbdbits.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Bbdmux.cpp

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\Bbdmux.obj"	"$(INTDIR)\Bbdmux.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\Bbdmux.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\demuxbuf.cpp

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\demuxbuf.obj"	"$(INTDIR)\demuxbuf.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\demuxbuf.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\my_fread.cpp

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\my_fread.obj"	"$(INTDIR)\my_fread.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\my_fread.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\output_aacfile.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\output_aacfile.obj"	"$(INTDIR)\output_aacfile.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\output_aacfile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\output_wavefile.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\output_wavefile.obj"	"$(INTDIR)\output_wavefile.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\output_wavefile.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\output_winwave.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\output_winwave.obj"	"$(INTDIR)\output_winwave.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\output_winwave.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ring_buffer.c

!IF  "$(CFG)" == "ac3dec - Win32 Release"


"$(INTDIR)\ring_buffer.obj"	"$(INTDIR)\ring_buffer.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ac3dec - Win32 Debug"


"$(INTDIR)\ring_buffer.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

