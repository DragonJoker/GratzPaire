project( GratzPaire )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Gratz Paire" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 2 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

if( APPLE )
	set( GTK_FOUND TRUE )
	set( GTK_LIBRARIES
		"-framework AppKit"
		"-framework QuartzCore"
		"-framework Metal"
	)
elseif ( WIN32 )
	set( GTK_FOUND TRUE )
else()
	find_package( GTK3 QUIET )
	find_package( X11 QUIET )
	if ( GTK3_FOUND )
		set( GTK_FOUND ${GTK3_FOUND} )
		set( GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS} )
		set( GTK_LIBRARIES ${GTK3_LIBRARIES} )
	else ()
		set( GTK_FOUND FALSE )
	endif ()
endif()

find_package( wxWidgets COMPONENTS core base aui adv )

if( wxWidgets_FOUND AND GTK_FOUND )
	set( ${PROJECT_NAME}_HDR_FILES
		Audio.hpp
		Card.hpp
		Game.hpp
		GratzPaire.hpp
		GratzPairePch.hpp
		GratzPairePrerequisites.hpp
		Hud.hpp
		Level.hpp
		MainFrame.hpp
		RenderPanel.hpp
		Sound.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		Audio.cpp
		Card.cpp
		Game.cpp
		GratzPaire.cpp
		GratzPairePch.cpp
		GratzPairePrerequisites.cpp
		Hud.cpp
		Level.cpp
		MainFrame.cpp
		RenderPanel.cpp
		Sound.cpp
	)
	source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
	source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )
	if ( WIN32 )
		find_rsc_file( ${PROJECT_NAME} bin )
		target_add_manifest( ${PROJECT_NAME} )
	endif ()
	add_target_min( ${PROJECT_NAME}
		bin
		"GratzPairePch.hpp"
		"GratzPairePch.cpp"
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			CXX_STANDARD 20
			CXX_EXTENSIONS OFF
	)
	target_include_directories( ${PROJECT_NAME} SYSTEM
		PRIVATE
			${wxWidgets_INCLUDE_DIRS}
			${GTK_INCLUDE_DIRS}
			${Fmod_INCLUDE_DIR}
	)
	target_link_libraries( ${PROJECT_NAME}
		PRIVATE
			ashes::ashesCommon
			ashes::ashes
			ashes::ashespp
			ashes::ashesVkRenderer
			sdw::ShaderAST
			sdw::ShaderWriter
			sdw::CompilerGlsl
			sdw::CompilerSpirV
			castor::CastorUtils
			castor::Castor3D
			${Fmod_LIBRARY}
			${wxWidgets_LIBRARIES}
			${GTK_LIBRARIES}
	)
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			${wxWidgets_DEFINITIONS}
	)
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
	copy_target_files_ex( ${PROJECT_NAME}
		GratzPaire
		""
		${DataFiles}
	)
else()
	if( NOT wxWidgets_FOUND )
		message( WARNING "wxWidgets libraries not found" )
	else()
		message( WARNING "GTK2 libraries not found" )
	endif()
endif ()
