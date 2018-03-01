# Microsoft Developer Studio Project File - Name="ChordEase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ChordEase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ChordEase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ChordEase.mak" CFG="ChordEase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ChordEase - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ChordEase - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ChordEase - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imagehlp.lib version.lib shlwapi.lib winmm.lib comctl32.lib htmlhelp.lib /nologo /subsystem:windows /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "ChordEase - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imagehlp.lib version.lib shlwapi.lib winmm.lib comctl32.lib htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ChordEase - Win32 Release"
# Name "ChordEase - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\ArrayEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ArrayEx.h
# End Source File
# Begin Source File

SOURCE=.\AuxiliaryDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\AuxiliaryDoc.h
# End Source File
# Begin Source File

SOURCE=.\Benchmark.cpp
# End Source File
# Begin Source File

SOURCE=.\Benchmark.h
# End Source File
# Begin Source File

SOURCE=.\BoundArray.h
# End Source File
# Begin Source File

SOURCE=.\ChangeLengthDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangeLengthDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChartUndoCodeData.h
# End Source File
# Begin Source File

SOURCE=.\ChartUndoCodes.h
# End Source File
# Begin Source File

SOURCE=.\ChartUndoTest.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartUndoTest.h
# End Source File
# Begin Source File

SOURCE=.\ChildDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChordBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordBar.h
# End Source File
# Begin Source File

SOURCE=.\ChordBarCtrlDef.h
# End Source File
# Begin Source File

SOURCE=.\ChordDictDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\ChordDictionaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordDictionaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChordDictUndoCodes.h
# End Source File
# Begin Source File

SOURCE=.\ChordEase.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordEase.h
# End Source File
# Begin Source File

SOURCE=.\ChordEase.rc
# End Source File
# Begin Source File

SOURCE=.\ChordEaseDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordEaseDoc.h
# End Source File
# Begin Source File

SOURCE=.\ChordEaseView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChordEaseView.h
# End Source File
# Begin Source File

SOURCE=.\ChordSubmenuDef.h
# End Source File
# Begin Source File

SOURCE=.\ChordVoicingDef.h
# End Source File
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.h
# End Source File
# Begin Source File

SOURCE=.\ColorStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\CritSec.h
# End Source File
# Begin Source File

SOURCE=.\CtrlResize.cpp
# End Source File
# Begin Source File

SOURCE=.\CtrlResize.h
# End Source File
# Begin Source File

SOURCE=.\DeviceBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceBar.h
# End Source File
# Begin Source File

SOURCE=.\DeviceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeviceDlg.h
# End Source File
# Begin Source File

SOURCE=.\DeviceDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\Diatonic.cpp
# End Source File
# Begin Source File

SOURCE=.\Diatonic.h
# End Source File
# Begin Source File

SOURCE=.\DLLWrap.cpp
# End Source File
# Begin Source File

SOURCE=.\DLLWrap.h
# End Source File
# Begin Source File

SOURCE=.\DocManagerEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DocManagerEx.h
# End Source File
# Begin Source File

SOURCE=.\DragVirtualListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DragVirtualListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DurationComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DurationComboBox.h
# End Source File
# Begin Source File

SOURCE=.\DurationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DurationDlg.h
# End Source File
# Begin Source File

SOURCE=.\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine.h
# End Source File
# Begin Source File

SOURCE=.\EngineMidi.cpp
# End Source File
# Begin Source File

SOURCE=.\EngineMidi.h
# End Source File
# Begin Source File

SOURCE=.\Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Event.h
# End Source File
# Begin Source File

SOURCE=.\FixedArray.h
# End Source File
# Begin Source File

SOURCE=.\FocusEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FocusEdit.h
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HelpIDs.h
# End Source File
# Begin Source File

SOURCE=.\HelpResMap.h
# End Source File
# Begin Source File

SOURCE=.\HeptatonicDef.h
# End Source File
# Begin Source File

SOURCE=.\Hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=.\Hyperlink.h
# End Source File
# Begin Source File

SOURCE=.\IconButton.cpp
# End Source File
# Begin Source File

SOURCE=.\IconButton.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\InsertChordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\InsertChordDlg.h
# End Source File
# Begin Source File

SOURCE=.\ListCtrlExSel.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlExSel.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainPaneDef.h
# End Source File
# Begin Source File

SOURCE=.\MainSizingBarDef.h
# End Source File
# Begin Source File

SOURCE=.\MainStatusPaneDef.h
# End Source File
# Begin Source File

SOURCE=.\MainToolDlgDef.h
# End Source File
# Begin Source File

SOURCE=.\MessageBoxCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageBoxCheck.h
# End Source File
# Begin Source File

SOURCE=.\Midi.h
# End Source File
# Begin Source File

SOURCE=.\MidiAssignPropsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiAssignPropsDlg.h
# End Source File
# Begin Source File

SOURCE=.\MidiAssignsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiAssignsDlg.h
# End Source File
# Begin Source File

SOURCE=.\MidiAssignsDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiCtrlrDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiCtrlrName.h
# End Source File
# Begin Source File

SOURCE=.\MidiDeviceID.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiDeviceID.h
# End Source File
# Begin Source File

SOURCE=.\MidiEventBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiEventBar.h
# End Source File
# Begin Source File

SOURCE=.\MidiEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiEventDlg.h
# End Source File
# Begin Source File

SOURCE=.\MidiEventDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiEventListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiEventListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MidiEventTypeDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiFile.h
# End Source File
# Begin Source File

SOURCE=.\MidiInst.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiInst.h
# End Source File
# Begin Source File

SOURCE=.\MidiNoteMapDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiNoteMapDlg.h
# End Source File
# Begin Source File

SOURCE=.\MidiNoteMapDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiPortID.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiPortID.h
# End Source File
# Begin Source File

SOURCE=.\MidiRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiRecord.h
# End Source File
# Begin Source File

SOURCE=.\MidiStatusDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiTarget.h
# End Source File
# Begin Source File

SOURCE=.\MidiTargetColDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiTargetCtrlTypeDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiTargetDef.h
# End Source File
# Begin Source File

SOURCE=.\MidiTargetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiTargetDlg.h
# End Source File
# Begin Source File

SOURCE=.\MidiWrap.cpp
# End Source File
# Begin Source File

SOURCE=.\MidiWrap.h
# End Source File
# Begin Source File

SOURCE=.\ModeDef.h
# End Source File
# Begin Source File

SOURCE=.\ModelessDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelessDlg.h
# End Source File
# Begin Source File

SOURCE=.\ModelessDlgPtr.h
# End Source File
# Begin Source File

SOURCE=.\MySizingControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\MySizingControlBar.h
# End Source File
# Begin Source File

SOURCE=.\MyToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\MyToolbar.h
# End Source File
# Begin Source File

SOURCE=.\Note.cpp
# End Source File
# Begin Source File

SOURCE=.\Note.h
# End Source File
# Begin Source File

SOURCE=.\NoteEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NoteEdit.h
# End Source File
# Begin Source File

SOURCE=.\NoteMapPropsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NoteMapPropsDlg.h
# End Source File
# Begin Source File

SOURCE=.\NotepadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NotepadDlg.h
# End Source File
# Begin Source File

SOURCE=.\NumEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NumEdit.h
# End Source File
# Begin Source File

SOURCE=.\NumSpin.cpp
# End Source File
# Begin Source File

SOURCE=.\NumSpin.h
# End Source File
# Begin Source File

SOURCE=.\OptionDef.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptionsInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsInfo.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPages.h
# End Source File
# Begin Source File

SOURCE=.\OptsChartDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsChartDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptsOtherDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsOtherDlg.h
# End Source File
# Begin Source File

SOURCE=.\OptsRecordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsRecordDlg.h
# End Source File
# Begin Source File

SOURCE=.\OutputNotesBar.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputNotesBar.h
# End Source File
# Begin Source File

SOURCE=.\OutputNotesStateDef.h
# End Source File
# Begin Source File

SOURCE=.\OutputNotesSubmenuDef.h
# End Source File
# Begin Source File

SOURCE=.\Part.cpp
# End Source File
# Begin Source File

SOURCE=.\Part.h
# End Source File
# Begin Source File

SOURCE=.\PartAutoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartAutoDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartBassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartBassDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartCompDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartCompDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartDef.h
# End Source File
# Begin Source File

SOURCE=.\PartFunctionDef.h
# End Source File
# Begin Source File

SOURCE=.\PartHarmonizerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartHarmonizerDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartLeadDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartLeadDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartLegacyDef.h
# End Source File
# Begin Source File

SOURCE=.\PartMidiTargetDef.h
# End Source File
# Begin Source File

SOURCE=.\PartMidiTargetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartMidiTargetDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartOutputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartOutputDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartPageDef.h
# End Source File
# Begin Source File

SOURCE=.\PartPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PartPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\PartPageView.cpp
# End Source File
# Begin Source File

SOURCE=.\PartPageView.h
# End Source File
# Begin Source File

SOURCE=.\PartPortDef.h
# End Source File
# Begin Source File

SOURCE=.\PartsBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PartsBar.h
# End Source File
# Begin Source File

SOURCE=.\PartsListColDef.h
# End Source File
# Begin Source File

SOURCE=.\PartsListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PartsListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PartsListView.cpp
# End Source File
# Begin Source File

SOURCE=.\PartsListView.h
# End Source File
# Begin Source File

SOURCE=.\Patch.cpp
# End Source File
# Begin Source File

SOURCE=.\Patch.h
# End Source File
# Begin Source File

SOURCE=.\PatchBar.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchBar.h
# End Source File
# Begin Source File

SOURCE=.\PatchDef.h
# End Source File
# Begin Source File

SOURCE=.\PatchDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchDoc.h
# End Source File
# Begin Source File

SOURCE=.\PatchGeneralDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchGeneralDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatchMetronomeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchMetronomeDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatchMidiTargetDef.h
# End Source File
# Begin Source File

SOURCE=.\PatchMidiTargetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchMidiTargetDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatchMidiTargetIter.h
# End Source File
# Begin Source File

SOURCE=.\PatchPageDef.h
# End Source File
# Begin Source File

SOURCE=.\PatchPageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchPageDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatchPortDef.h
# End Source File
# Begin Source File

SOURCE=.\PatchPortIterator.h
# End Source File
# Begin Source File

SOURCE=.\PatchSyncDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchSyncDlg.h
# End Source File
# Begin Source File

SOURCE=.\PatchUndoTest.cpp
# End Source File
# Begin Source File

SOURCE=.\PatchUndoTest.h
# End Source File
# Begin Source File

SOURCE=.\PathListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PathListDlg.h
# End Source File
# Begin Source File

SOURCE=.\PathStr.cpp
# End Source File
# Begin Source File

SOURCE=.\PathStr.h
# End Source File
# Begin Source File

SOURCE=.\Persist.cpp
# End Source File
# Begin Source File

SOURCE=.\Persist.h
# End Source File
# Begin Source File

SOURCE=.\PersistDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PersistDlg.h
# End Source File
# Begin Source File

SOURCE=.\PianoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PianoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\PianoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PianoDlg.h
# End Source File
# Begin Source File

SOURCE=.\PianoSizeDef.h
# End Source File
# Begin Source File

SOURCE=.\PianoSizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PianoSizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\PianoStateDef.h
# End Source File
# Begin Source File

SOURCE=.\PopupCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupCombo.h
# End Source File
# Begin Source File

SOURCE=.\PopupEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PopupEdit.h
# End Source File
# Begin Source File

SOURCE=.\PositionDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PositionDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\QuantDef.h
# End Source File
# Begin Source File

SOURCE=.\RandList.cpp
# End Source File
# Begin Source File

SOURCE=.\RandList.h
# End Source File
# Begin Source File

SOURCE=.\Range.h
# End Source File
# Begin Source File

SOURCE=.\RecordPlayerColDef.h
# End Source File
# Begin Source File

SOURCE=.\RecordPlayerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordPlayerDlg.h
# End Source File
# Begin Source File

SOURCE=.\RefPtr.h
# End Source File
# Begin Source File

SOURCE=.\ReportCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RingBuf.h
# End Source File
# Begin Source File

SOURCE=.\Round.h
# End Source File
# Begin Source File

SOURCE=.\SafeHandle.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeHandle.h
# End Source File
# Begin Source File

SOURCE=.\Scale.cpp
# End Source File
# Begin Source File

SOURCE=.\Scale.h
# End Source File
# Begin Source File

SOURCE=.\scbarcf.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarcf.h
# End Source File
# Begin Source File

SOURCE=.\scbarg.cpp
# End Source File
# Begin Source File

SOURCE=.\scbarg.h
# End Source File
# Begin Source File

SOURCE=.\ScrollDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollDlg.h
# End Source File
# Begin Source File

SOURCE=.\SectionListColDef.h
# End Source File
# Begin Source File

SOURCE=.\SectionListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SectionListDlg.h
# End Source File
# Begin Source File

SOURCE=.\SectionPropsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SectionPropsDlg.h
# End Source File
# Begin Source File

SOURCE=.\sizecbar.cpp
# End Source File
# Begin Source File

SOURCE=.\sizecbar.h
# End Source File
# Begin Source File

SOURCE=.\Song.cpp
# End Source File
# Begin Source File

SOURCE=.\Song.h
# End Source File
# Begin Source File

SOURCE=.\SongCommandDef.h
# End Source File
# Begin Source File

SOURCE=.\SongPropsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SongPropsDlg.h
# End Source File
# Begin Source File

SOURCE=.\SongState.cpp
# End Source File
# Begin Source File

SOURCE=.\SongState.h
# End Source File
# Begin Source File

SOURCE=.\SpinNumEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\SpinNumEdit.h
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.h
# End Source File
# Begin Source File

SOURCE=.\Statistics.cpp
# End Source File
# Begin Source File

SOURCE=.\Statistics.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SteadyStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\SteadyStatic.h
# End Source File
# Begin Source File

SOURCE=.\StringUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\StringUtil.h
# End Source File
# Begin Source File

SOURCE=.\TabbedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabbedDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TabCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\ThreadsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ThreadsDlgColDef.h
# End Source File
# Begin Source File

SOURCE=.\TokenFile.cpp
# End Source File
# Begin Source File

SOURCE=.\TokenFile.h
# End Source File
# Begin Source File

SOURCE=.\TransposeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TransposeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Undoable.cpp
# End Source File
# Begin Source File

SOURCE=.\Undoable.h
# End Source File
# Begin Source File

SOURCE=.\UndoCodeData.h
# End Source File
# Begin Source File

SOURCE=.\UndoCodes.h
# End Source File
# Begin Source File

SOURCE=.\UndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoManager.h
# End Source File
# Begin Source File

SOURCE=.\UndoState.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoState.h
# End Source File
# Begin Source File

SOURCE=.\UndoTest.cpp
# End Source File
# Begin Source File

SOURCE=.\UndoTest.h
# End Source File
# Begin Source File

SOURCE=.\VelocityPalette.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\Win32Console.cpp
# End Source File
# Begin Source File

SOURCE=.\Win32Console.h
# End Source File
# Begin Source File

SOURCE=.\WinAppCK.cpp
# End Source File
# Begin Source File

SOURCE=.\WinAppCK.h
# End Source File
# Begin Source File

SOURCE=.\WinAppCKRegTypes.h
# End Source File
# Begin Source File

SOURCE=.\WObject.h
# End Source File
# Begin Source File

SOURCE=.\WorkerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkerThread.h
# End Source File
# Begin Source File

SOURCE=.\Wrapx64.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ChordBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ChordEase.ico
# End Source File
# Begin Source File

SOURCE=.\res\ChordEase.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ChordEaseDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\ChordEasePatch.ico
# End Source File
# Begin Source File

SOURCE=.\res\dragmulti.cur
# End Source File
# Begin Source File

SOURCE=.\res\dragsingle.cur
# End Source File
# Begin Source File

SOURCE=.\res\header_sort.bmp
# End Source File
# Begin Source File

SOURCE=.\res\RecPlayPause.ico
# End Source File
# Begin Source File

SOURCE=.\res\RecPlayPlay.ico
# End Source File
# Begin Source File

SOURCE=.\res\RecPlayStop.ico
# End Source File
# Begin Source File

SOURCE=.\res\scale_ba.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
