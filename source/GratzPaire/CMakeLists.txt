project( GratzPaire )

include_directories( ../GuiCommon/Src )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Gratz Paire" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

if( NOT WIN32 )
	find_package( GTK2 )
	set( X11Libraries X11 )
else()
	set( GTK2_FOUND TRUE )
endif()

find_package( wxWidgets COMPONENTS core base aui adv )

if( wxWidgets_FOUND AND GTK2_FOUND )
	include_directories( ${GTK2_INCLUDE_DIRS} )
	include_directories( ${wxWidgets_INCLUDE_DIRS} )
	string( COMPARE EQUAL "${wxWidgets_USE_DEBUG}" "ON" IsWxDebug )
	set( wxWidgets_CXXFLAGS "${wxWidgets_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES" )

	if( NOT WIN32 )
		set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXGTK__" )
	endif()

	if( IsWxDebug )
		set( wxWidgets_CXXFLAGS "${wxWidgets_CXXFLAGS} -D__WXDEBUG__" )
	endif()
	set( wxWidgetsLibraries "" )
	foreach( Lib ${wxWidgets_LIBRARIES} )
		if( wxWidgetsLibraries )
			set( wxWidgetsLibraries "${wxWidgetsLibraries}|${Lib}" )
		else()
			set( wxWidgetsLibraries "${Lib}" )
		endif()
	endforeach()
	foreach( Lib ${GTK2_LIBRARIES} )
		if( wxWidgetsLibraries )
			set( wxWidgetsLibraries "${wxWidgetsLibraries}|${Lib}" )
		else()
			set( wxWidgetsLibraries "${Lib}" )
		endif()
	endforeach()

	add_target(
		${PROJECT_NAME}
		bin
		""
		"${PairesMinLibraries};${wxWidgetsLibraries};${X11Libraries}"
		"GratzPairePch.hpp"
		"GratzPairePch.cpp"
		"${wxWidgets_CFLAGS}"
		"${wxWidgets_CXXFLAGS}"
	)
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Outputs" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	castor3d_copy_files(
		${PROJECT_NAME}
		${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}
		${PROJECTS_BINARIES_OUTPUT_DIR_RELWITHDEBINFO}
		${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}
	)

	if ( WIN32 )
		include( InstallRequiredSystemLibraries )
		get_filename_component( FmodLibPath ${Fmod_LIBRARY} PATH )
		if ( ${PROJECTS_PLATFORM} STREQUAL "x86" )
			set( FmodLibPath ${FmodLibPath}/fmod.dll )
		else ()
			set( FmodLibPath ${FmodLibPath}/fmod64.dll )
		endif ()
		copy_dll( ${PROJECT_NAME} ${FmodLibPath} ${FmodLibPath} )
	endif ()

	file(
		GLOB
			DataFiles
			${CMAKE_CURRENT_SOURCE_DIR}/Data/*.zip
	)

	copy_target_files( ${PROJECT_NAME} "" ${DataFiles} )
else()
	if( NOT wxWidgets_FOUND )
		message( WARNING "wxWidgets libraries not found" )
	else()
		message( WARNING "GTK2 libraries not found" )
	endif()
endif ()
