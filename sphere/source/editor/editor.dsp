# Microsoft Developer Studio Project File - Name="editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=editor - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak" CFG="editor - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "editor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "editor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "editor - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "editor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output\Release"
# PROP Intermediate_Dir "output\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /O1 /I "../../third-party-msvc/include" /I "../../third-party-msvc/include/smjs/" /D "NDEBUG" /D "STRIC" /D "INCLUDE_DEPRECATED_FEATURES" /D "XP_PC" /D "XP_WIN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VC_EXTRALEAN" /D for="if (0) ; else for" /D "ZLIB_DLL" /D "MNG_USE_DLL" /D "MNG_SKIP_LCMS" /D "MNG_SKIP_IJG6B" /D "NOMINMAX" /D "TABBED_WINDOW_LIST" /D "I_SUCK" /D "_AFXDLL" /FAs /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 htmlhelp.lib corona.lib audiere.lib winmm.lib js32.lib zlib.lib libmng.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:"../../third-party-msvc/lib" /opt:nowin98
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=install_editor.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output\Debug"
# PROP Intermediate_Dir "output\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /Gi /GX /ZI /Od /I "../../third-party-msvc/include" /D "_DEBUG" /D "STRICT" /D "_AFXDLL" /D "INCLUDE_DEPRECATED_FEATURES" /D "XP_PC" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VC_EXTRALEAN" /D for="if (0) ; else for" /D "ZLIB_DLL" /D "MNG_USE_DLL" /D "MNG_SKIP_LCMS" /D "MNG_SKIP_IJG6B" /D "NOMINMAX" /FAs /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 htmlhelp.lib corona.lib audiere.lib winmm.lib js32.lib zlib.lib libmng.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../../third-party-msvc/lib"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "editor___Win32_Profile"
# PROP BASE Intermediate_Dir "editor___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output\Profile"
# PROP Intermediate_Dir "output\Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp1 /MTd /W3 /Gm /Gi /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VC_EXTRALEAN" /D "STRICT" /FAs /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MDd /W3 /Gm /Gi /GX /ZI /Od /I "../../third-party-msvc/include" /D "_DEBUG" /D "STRIC" /D "_AFXDLL" /D "INCLUDE_DEPRECATED_FEATURES" /D "XP_PC" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "VC_EXTRALEAN" /D "ZLIB_DLL" /D "MNG_USE_DLL" /D "MNG_SKIP_LCMS" /D "MNG_SKIP_IJG6B" /D "NOMINMAX" /D "TABBED_WINDOW_LIST" /D "I_SUCK" /FAs /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 htmlhelp.lib corona.lib audiere.lib winmm.lib js32.lib zlib.lib libmng.lib wsock32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"../../third-party-msvc/lib"

!ENDIF 

# Begin Target

# Name "editor - Win32 Release"
# Name "editor - Win32 Debug"
# Name "editor - Win32 Profile"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Audio.hpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Clipboard.hpp
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\Configuration.hpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.hpp
# End Source File
# Begin Source File

SOURCE=.\DIBSection.cpp
# End Source File
# Begin Source File

SOURCE=.\DIBSection.hpp
# End Source File
# Begin Source File

SOURCE=.\Editor.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor.hpp
# End Source File
# Begin Source File

SOURCE=.\EditRange.cpp
# End Source File
# Begin Source File

SOURCE=.\EditRange.hpp
# End Source File
# Begin Source File

SOURCE=.\filename_comparer.cpp
# End Source File
# Begin Source File

SOURCE=.\filename_comparer.hpp
# End Source File
# Begin Source File

SOURCE=.\FileTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTypes.hpp
# End Source File
# Begin Source File

SOURCE=.\ImageRender.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageRender.hpp
# End Source File
# Begin Source File

SOURCE=.\Keys.cpp
# End Source File
# Begin Source File

SOURCE=.\Keys.hpp
# End Source File
# Begin Source File

SOURCE=.\Keys.table
# End Source File
# Begin Source File

SOURCE=.\LineSorter.cpp
# End Source File
# Begin Source File

SOURCE=.\LineSorter.hpp
# End Source File
# Begin Source File

SOURCE=.\MDITabs.cpp
# End Source File
# Begin Source File

SOURCE=.\MDITabs.h
# End Source File
# Begin Source File

SOURCE=.\Package.cpp
# End Source File
# Begin Source File

SOURCE=.\Package.hpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.hpp
# End Source File
# Begin Source File

SOURCE=.\Scripting.cpp
# End Source File
# Begin Source File

SOURCE=.\Scripting.hpp
# End Source File
# Begin Source File

SOURCE=.\SwatchServer.cpp
# End Source File
# Begin Source File

SOURCE=.\SwatchServer.hpp
# End Source File
# Begin Source File

SOURCE=.\translate.cpp
# End Source File
# Begin Source File

SOURCE=.\translate.hpp
# End Source File
# Begin Source File

SOURCE=..\engine\win32\win32_sphere_config.cpp
# End Source File
# Begin Source File

SOURCE=..\engine\win32\win32_sphere_config.hpp
# End Source File
# Begin Source File

SOURCE=.\Zoomer.hpp
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resources\animation.ico
# End Source File
# Begin Source File

SOURCE=.\resources\browse.ico
# End Source File
# Begin Source File

SOURCE=.\resources\editor.rc
# End Source File
# Begin Source File

SOURCE=.\resources\entity_person.ico
# End Source File
# Begin Source File

SOURCE=.\resources\entity_person_old.ico
# End Source File
# Begin Source File

SOURCE=.\resources\entity_trigger.ico
# End Source File
# Begin Source File

SOURCE=.\resources\font.ico
# End Source File
# Begin Source File

SOURCE=.\resources\game.ico
# End Source File
# Begin Source File

SOURCE=.\resources\image.ico
# End Source File
# Begin Source File

SOURCE=.\resources\image_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_circle.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_colorpicker.cur
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_colorpicker.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_ellipse.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_fill.cur
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_fill.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_freeselection.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_line.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_pencil.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_rectangle.ico
# End Source File
# Begin Source File

SOURCE=.\resources\imagetool_selection.ico
# End Source File
# Begin Source File

SOURCE=.\resources\layer_invisible.ico
# End Source File
# Begin Source File

SOURCE=.\resources\layer_visible.ico
# End Source File
# Begin Source File

SOURCE=.\resources\map.ico
# End Source File
# Begin Source File

SOURCE=.\resources\map_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\resources\maptool.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_1x1.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_3x3.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_5x5.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_copyarea.cur
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_copyarea.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_copyentity.cur
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_copyentity.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_fillarea.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_fillrect.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_moveentity.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_obs_delete.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_obs_move_pt.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_obs_segment.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_paste.cur
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_paste.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_pasteentity.cur
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_pasteentity.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_selecttile.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_zoneadd.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_zonedelete.ico
# End Source File
# Begin Source File

SOURCE=.\resources\maptool_zoneedit.ico
# End Source File
# Begin Source File

SOURCE=.\resources\music.ico
# End Source File
# Begin Source File

SOURCE=.\resources\project.ico
# End Source File
# Begin Source File

SOURCE=.\resources\pv_closed_folder.ico
# End Source File
# Begin Source File

SOURCE=.\resources\pv_game_settings.ico
# End Source File
# Begin Source File

SOURCE=.\resources\pv_open_folder.ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resources\script.ico
# End Source File
# Begin Source File

SOURCE=.\resources\sde.ico
# End Source File
# Begin Source File

SOURCE=.\resources\sound.ico
# End Source File
# Begin Source File

SOURCE=.\resources\spriteset.ico
# End Source File
# Begin Source File

SOURCE=.\resources\tileset.ico
# End Source File
# Begin Source File

SOURCE=.\resources\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\resources\unknown.ico
# End Source File
# Begin Source File

SOURCE=.\resources\volumebar.bmp
# End Source File
# Begin Source File

SOURCE=.\resources\windowstyle.ico
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ConsoleWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\HScrollWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\HScrollWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ProjectWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\ScrollWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\VScrollWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\VScrollWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\WindowCommands.hpp
# End Source File
# End Group
# Begin Group "Document Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AnimationWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\BrowseWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\DocumentWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\FontWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\FontWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\ImageWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageWindow.hpp
# End Source File
# Begin Source File

SOURCE=..\common\LogWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\MapWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MapWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\SaveableDocumentWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SaveableDocumentWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\ScriptWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\SoundWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\WindowStyleWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowStyleWindow.hpp
# End Source File
# End Group
# Begin Group "Views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AlphaView.cpp
# End Source File
# Begin Source File

SOURCE=.\AlphaView.hpp
# End Source File
# Begin Source File

SOURCE=.\ColorView.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorView.hpp
# End Source File
# Begin Source File

SOURCE=.\ImageView.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageView.hpp
# End Source File
# Begin Source File

SOURCE=.\LayerView.cpp
# End Source File
# Begin Source File

SOURCE=.\LayerView.hpp
# End Source File
# Begin Source File

SOURCE=.\MapView.cpp
# End Source File
# Begin Source File

SOURCE=.\MapView.hpp
# End Source File
# Begin Source File

SOURCE=.\PaletteView.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteView.hpp
# End Source File
# Begin Source File

SOURCE=.\RGBAView.cpp
# End Source File
# Begin Source File

SOURCE=.\RGBAView.hpp
# End Source File
# Begin Source File

SOURCE=.\SpriteBaseView.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteBaseView.hpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetView.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetView.hpp
# End Source File
# Begin Source File

SOURCE=.\TileObstructionView.cpp
# End Source File
# Begin Source File

SOURCE=.\TileObstructionView.hpp
# End Source File
# Begin Source File

SOURCE=.\TilesetEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\TilesetEditView.hpp
# End Source File
# Begin Source File

SOURCE=.\TilesetView.cpp
# End Source File
# Begin Source File

SOURCE=.\TilesetView.hpp
# End Source File
# End Group
# Begin Group "Palettes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FontPreviewPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\FontPreviewPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\ImageToolPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageToolPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\MapToolPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\MapToolPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\PaletteWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteWindow.hpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetAnimationPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetAnimationPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetImagesPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritesetImagesPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\SwatchPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\SwatchPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\TilePalette.cpp
# End Source File
# Begin Source File

SOURCE=.\TilePalette.hpp
# End Source File
# Begin Source File

SOURCE=.\TilePreviewPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\TilePreviewPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\ToolPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolPalette.hpp
# End Source File
# Begin Source File

SOURCE=.\WindowStylePreviewPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowStylePreviewPalette.hpp
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AdjustBordersDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AdjustBordersDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\CheckListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckListDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ColorAdjustDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorAdjustDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ConvolveListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ConvolveListDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\EntityListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityListDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\EntityPersonDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityPersonDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\EntityTriggerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EntityTriggerDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\FileDialogs.hpp
# End Source File
# Begin Source File

SOURCE=.\FontGradientDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FontGradientDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\GameSettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSettingsDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\LayerPropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LayerPropertiesDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\LineSorterDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LineSorterDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ListDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\MapPropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MapPropertiesDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\MoveLayerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveLayerDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\NewMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NewMapDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\NewProjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NewProjectDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\NumberDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ResizeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizeDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\RotateDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RotateDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\SpritePropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SpritePropertiesDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\StringDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StringDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\TileObstructionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TileObstructionDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\TilePropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TilePropertiesDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\TilesetSelectionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\TilesetSelectionDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\WindowStylePropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowStylePropertiesDialog.hpp
# End Source File
# Begin Source File

SOURCE=.\ZoneEditDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneEditDialog.hpp
# End Source File
# End Group
# Begin Group "Sphere Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\AnimationFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AnimationFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\common\common_palettes.cpp
# End Source File
# Begin Source File

SOURCE=..\common\common_palettes.hpp
# End Source File
# Begin Source File

SOURCE=..\common\configfile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\configfile.hpp
# End Source File
# Begin Source File

SOURCE=..\common\convolve.cpp
# End Source File
# Begin Source File

SOURCE=..\common\convolve.hpp
# End Source File
# Begin Source File

SOURCE=..\common\DefaultFile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\DefaultFile.hpp
# End Source File
# Begin Source File

SOURCE=..\common\DefaultFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\common\DefaultFileSystem.hpp
# End Source File
# Begin Source File

SOURCE=..\common\endian.hpp
# End Source File
# Begin Source File

SOURCE=..\common\ends_with.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ends_with.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Entities.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Entities.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Filters.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Filters.hpp
# End Source File
# Begin Source File

SOURCE=..\common\FLICAnimation.cpp
# End Source File
# Begin Source File

SOURCE=..\common\FLICAnimation.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Font.hpp
# End Source File
# Begin Source File

SOURCE=..\common\hsi.cpp
# End Source File
# Begin Source File

SOURCE=..\common\hsi.hpp
# End Source File
# Begin Source File

SOURCE=..\common\IAnimation.hpp
# End Source File
# Begin Source File

SOURCE=..\common\IFile.hpp
# End Source File
# Begin Source File

SOURCE=..\common\IFileSystem.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Image32.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Image32.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Interface.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Layer.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Layer.hpp
# End Source File
# Begin Source File

SOURCE=..\common\LogDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogDispatcher.hpp
# End Source File
# Begin Source File

SOURCE=..\common\LogEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\common\LogEvent.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Map.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Map.hpp
# End Source File
# Begin Source File

SOURCE=..\common\minmax.hpp
# End Source File
# Begin Source File

SOURCE=..\common\MNGAnimation.cpp
# End Source File
# Begin Source File

SOURCE=..\common\MNGAnimation.hpp
# End Source File
# Begin Source File

SOURCE=..\common\ObstructionMap.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ObstructionMap.hpp
# End Source File
# Begin Source File

SOURCE=..\common\packed.hpp
# End Source File
# Begin Source File

SOURCE=..\common\packed_struct.h
# End Source File
# Begin Source File

SOURCE=..\common\Playlist.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Playlist.hpp
# End Source File
# Begin Source File

SOURCE=..\common\primitives.hpp
# End Source File
# Begin Source File

SOURCE=..\common\resample.cpp
# End Source File
# Begin Source File

SOURCE=..\common\resample.hpp
# End Source File
# Begin Source File

SOURCE=..\common\rgb.hpp
# End Source File
# Begin Source File

SOURCE=..\common\sphere_version.h
# End Source File
# Begin Source File

SOURCE=..\common\spk.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Spriteset.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Spriteset.hpp
# End Source File
# Begin Source File

SOURCE=..\common\str_util.cpp
# End Source File
# Begin Source File

SOURCE=..\common\str_util.hpp
# End Source File
# Begin Source File

SOURCE=..\common\strcmp_ci.cpp
# End Source File
# Begin Source File

SOURCE=..\common\strcmp_ci.hpp
# End Source File
# Begin Source File

SOURCE=..\common\system.cpp
# End Source File
# Begin Source File

SOURCE=..\common\system.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Tile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Tile.hpp
# End Source File
# Begin Source File

SOURCE=..\common\Tileset.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Tileset.hpp
# End Source File
# Begin Source File

SOURCE=..\common\WindowStyle.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WindowStyle.hpp
# End Source File
# Begin Source File

SOURCE=..\common\write_mng.cpp
# End Source File
# Begin Source File

SOURCE=..\common\write_mng.hpp
# End Source File
# End Group
# Begin Group "Scripting"

# PROP Default_Filter ""
# Begin Group "env"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\engine\win32\win32_time.cpp
# End Source File
# Begin Source File

SOURCE=..\engine\win32\win32_time.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\engine\time.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\MapScripting.cpp
# End Source File
# Begin Source File

SOURCE=.\MapScripting.hpp
# End Source File
# Begin Source File

SOURCE=.\ss_functions.table
# End Source File
# End Group
# End Target
# End Project
