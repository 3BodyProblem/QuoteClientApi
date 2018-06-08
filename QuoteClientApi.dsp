# Microsoft Developer Studio Project File - Name="QuoteClientApi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=QuoteClientApi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuoteClientApi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuoteClientApi.mak" CFG="QuoteClientApi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuoteClientApi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "QuoteClientApi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuoteClientApi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QUOTECLIENTAPI_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QUOTECLIENTAPI_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"E:\QWinQuoteClientApi_DEMO_TaiwanMrHuang\QuoteClientApi.dll"

!ELSEIF  "$(CFG)" == "QuoteClientApi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QUOTECLIENTAPI_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QUOTECLIENTAPI_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"E:\QWinQuoteClientApi_DEMO_TaiwanMrHuang\QuoteClientApi.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "QuoteClientApi - Win32 Release"
# Name "QuoteClientApi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\QuoteClientApi.cpp
# End Source File
# Begin Source File

SOURCE=.\QuoteClientApi.def
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\QuoteClientApi.h
# End Source File
# Begin Source File

SOURCE=.\QuoteCltDef.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "SrvUnit"

# PROP Default_Filter ""
# Begin Group "DataClient"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\DataClient\DataClient.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\DataClient\DataClient.h
# End Source File
# End Group
# Begin Group "GlobalIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\GlobalIO\GlobalIO.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\GlobalIO\GlobalIO.h
# End Source File
# End Group
# Begin Group "OptionIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\OptionIO\MOptionIO.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\OptionIO\MOptionIO.h
# End Source File
# End Group
# Begin Group "MLogUnit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\MLogUnit\MLogUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\MLogUnit\MLogUnit.h
# End Source File
# End Group
# Begin Group "MPublic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\MPublic\MInterface.hpp
# End Source File
# End Group
# Begin Group "SubDll"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrvUnit\SubDll\Base4XDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\Base4XDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MCnfDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MCnfDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MCnfOptDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MCnfOptDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MData4XDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MData4XDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDataIO.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDataIO.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDataProc.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDataProc.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDllMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MDllMgr.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSHL2Dll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSHL2Dll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MShOptDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MShOptDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MShSEDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MShSEDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSZL2Dll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSZL2Dll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSzOptDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSzOptDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSzSEDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MSzSEDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MVPlat.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MVPlat.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MZjOptDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MZjOptDll.h
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MZjqhDll.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvUnit\SubDll\MZjqhDll.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
