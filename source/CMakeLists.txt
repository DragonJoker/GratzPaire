cmake_minimum_required( VERSION 2.8 )

#--------------------------------------------------------------------------------------------------
#	Initial configurations
#--------------------------------------------------------------------------------------------------
# Set project name, used in folders and in workspace creation
set( MAIN_PROJECT_NAME "GratzPaire" )

# Set project version numbers
set( VERSION_MAJOR 1 )
set( VERSION_MINOR 0 )
set( VERSION_BUILD 0 )
set( VERSION_YEAR 2017 )

# Used to look for external modules
set( CMAKE_MODULE_PATH
	${CMAKE_SOURCE_DIR}/CMake
	${CMAKE_SOURCE_DIR}/CMake/Modules
	${CMAKE_SOURCE_DIR}/CMake/Toolchains
)
set( CMAKE_TEMPLATES_DIR ${CMAKE_SOURCE_DIR}/CMake/Templates )
# Experimental Precompiled headers support for GCC
include( PCHSupport )

set( CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "The configuration types" FORCE )

# Declare the project
project( ${MAIN_PROJECT_NAME} )

include( Setup )
include( Project )
include( CompilerVersion )
include( UnitTest )
include( CompilationFlags )
include( AStyleUtils )
include( ExternalDependencies )

# Organize projects into folders
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

set( COMPILER_FOLDER ${COMPILER} )

if ( ${COMPILER_FOLDER} STREQUAL "vc15" )
	set( COMPILER_FOLDER "vc14" )
endif ()

#--------------------------------------------------------------------------------------------------
#	Looking for needed libraries
#--------------------------------------------------------------------------------------------------
if ( CMAKE_COMPILER_IS_GNUCXX )
	DumpCompilerVersion( PAIRES_COMPILER_VERSION )
	msg_debug( "GCC version : ${PAIRES_COMPILER_VERSION}" )
	if ( ${PAIRES_COMPILER_VERSION} LESS "49" )
		message( FATAL_ERROR "Your compiler is too old, consider an update." )
	endif ()
elseif ( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )
	DumpCompilerVersion( PAIRES_COMPILER_VERSION )
	msg_debug( "clang version : ${PAIRES_COMPILER_VERSION}" )
elseif ( MSVC )
	if ( MSVC60 OR MSVC70 OR MSVC71 OR MSVC80 OR MSVC10 OR MSVC11 OR MSVC12 )
		message( FATAL_ERROR "Your compiler is too old, consider an update." )
	endif ()
else ()
	message( FATAL_ERROR "Unsupported compiler." )
endif ()

find_package( Doxygen )
include( Doxygen )

#--------------------------------------------------------------------------------------------------
#	Adding include dirs to include search path
#--------------------------------------------------------------------------------------------------
include_directories( GameEngine/Src )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/GameEngine/Src )

if( DOXYGEN_FOUND )
	message( STATUS "+ Found Doxygen (version ${DOXYGEN_VERSION})" )
endif ()

#--------------------------------------------------------------------------------------------------
#	Normalising libraries names
#--------------------------------------------------------------------------------------------------
msg_debug( "CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
msg_debug( "CMAKE_C_FLAGS    : ${CMAKE_C_FLAGS}" )
msg_debug( "CMAKE_CXX_FLAGS  : ${CMAKE_CXX_FLAGS}" )

set( BinsDependencies
	GameEngine
)

#--------------------------------------------------------------------------------------------------
#	MSVC specifics
#--------------------------------------------------------------------------------------------------
if( MSVC )
	# We remove the existing /Zm flag to replace it by /Zm300
	string( REGEX REPLACE ".*(/Zm[0-9]+).*" "\\1" ZmFlag ${CMAKE_CXX_FLAGS} )
	if( (NOT "${ZmFlag}" STREQUAL "") AND (NOT "${CMAKE_CXX_FLAGS}" STREQUAL "${ZmFlag}") )
		string( REPLACE ${ZmFlag} "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	endif()
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm300" )
endif()

add_definitions(
 -DVERSION_MAJOR=${VERSION_MAJOR}
 -DVERSION_MINOR=${VERSION_MINOR}
 -DVERSION_BUILD=${VERSION_BUILD}
)

option( PAIRES_BUILDGRP_SETUP "Build setup projects" FALSE )

# Small macro to add subdirectory files to current target source and header files
macro( parse_subdir_files _FOLDER _GROUP )
	file( GLOB _FOLDER_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${_FOLDER}/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${_FOLDER}/*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${_FOLDER}/*.inl
 )
	file( GLOB _FOLDER_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${_FOLDER}/*.c
		${CMAKE_CURRENT_SOURCE_DIR}/Src/${_FOLDER}/*.cpp
 )
	source_group( "Header Files\\${_GROUP}" FILES ${_FOLDER_HEADER_FILES} )
	source_group( "Source Files\\${_GROUP}" FILES ${_FOLDER_SOURCE_FILES} )
endmacro()

find_package( Castor3D COMPONENTS CastorUtils CastorTest Castor3D GlslWriter )

if ( Castor3D_FOUND )
	include_directories( ${Castor3D_INCLUDE_DIRS} )
	set( CastorUtilsLibraries "")
	foreach(Lib ${Castor3D_CastorUtils_LIBRARIES})
		if (CastorUtilsLibraries)
			set(CastorUtilsLibraries "${CastorUtilsLibraries}|${Lib}")
		else()
			set(CastorUtilsLibraries "${Lib}")
		endif()
	endforeach()
	set( Castor3DLibraries "")
	foreach(Lib ${Castor3D_Castor3D_LIBRARIES})
		if (Castor3DLibraries)
			set(Castor3DLibraries "${Castor3DLibraries}|${Lib}")
		else()
			set(Castor3DLibraries "${Lib}")
		endif()
	endforeach()
	set( GlslWriterLibraries "")
	foreach(Lib ${Castor3D_GlslWriter_LIBRARIES})
		if (GlslWriterLibraries)
			set(GlslWriterLibraries "${GlslWriterLibraries}|${Lib}")
		else()
			set(GlslWriterLibraries "${Lib}")
		endif()
	endforeach()

	set( PairesMinLibraries
		${CastorUtilsLibraries}
		${Castor3DLibraries}
		${GlslWriterLibraries}
	)

	if ( MSVC OR NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
		if ( PAIRES_BUILDGRP_TEST )
			enable_testing()
		endif ()

		if ( ${PAIRES_BUILDGRP_SETUP} )
			if ( ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" ) OR ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" ) )
				get_filename_component( PARENT_DIR ${CMAKE_SOURCE_DIR} PATH )
				file( MAKE_DIRECTORY "${PROJECTS_SETUP_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}" )
				set( CPACK_PACKAGE_NAME "GratzPaire" )
				set( CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}" )
				set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gratz-Paires" )
				set( CPACK_PACKAGE_DESCRIPTION "Allows Gratz-Paires developments" )
				set( CPACK_PACKAGE_RELEASE 1 )
				set( CPACK_PACKAGE_CONTACT "dragonjoker59@hotmail.com" )
				set( CPACK_PACKAGE_VENDOR "DragonJoker" )
				set( CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${PROJECTS_PLATFORM}" )
				set( CPACK_OUTPUT_FILE_PREFIX "${PROJECTS_SETUP_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}" )
				set( CPACK_RESOURCE_FILE_LICENSE "${PARENT_DIR}/MIT.txt" )
				set( CMAKE_SYSTEM_ARCHITECTURE ${PROJECTS_PLATFORM} )

				if ( "${PROJECTS_PACKAGE_WIX}" STREQUAL "ON" )
					set( CPACK_GENERATOR "WIX" )
					set( CPACK_PACKAGE_INSTALL_DIRECTORY "ExiledFromPaires" )
					set( CPACK_WIX_UPGRADE_GUID "AEED9436-25CC-400A-9DBA-DE7627A5F09E" )
					set( CPACK_WIX_PRODUCT_GUID "21C5DEC3-31B0-46E2-BC62-CC2D834FDD43" )
					set( CPACK_WIX_PRODUCT_ICON "${PARENT_DIR}/install/castor.ico" )
					set( CPACK_WIX_UI_DIALOG "${PARENT_DIR}/install/dialog.bmp" )
					set( CPACK_WIX_UI_BANNER "${PARENT_DIR}/install/banner.bmp" )
					set( CPACK_WIX_PROGRAM_MENU_FOLDER "Gratz-Paires" )
					set( CPACK_WIX_CULTURES "fr-FR;en-EN" )
					include(CPack)
				elseif ( "${PROJECTS_PACKAGE_DEB}" STREQUAL "ON" )
					set( CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} )
					set( CPACK_DEB_COMPONENT_INSTALL ON )
					set( CPACK_GENERATOR "DEB" )
					set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Sylvain Doremus" )
					set( CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}libfreetype6 (>= 2.5.2), libfreeimage3 (>= 3.15.4), zlib1g (>= 1.2.8), Castor3D (>= 0.9.0)" )
					set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_ARCHITECTURE}" )
					set( CPACK_DEBIAN_PACKAGE_SECTION "devel" )
					set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
					include(CPack)
				elseif ( "${PROJECTS_PACKAGE_RPM}" STREQUAL "ON" )
					set( ${Castor_PACKAGE_DIR} ${CMAKE_BINARY_DIR}/_CPack_Packages/Linux/RPM )
					set( CPACK_RPM_COMPONENT_INSTALL ON )
					set( CPACK_RPM_PACKAGE_SUMMARY "ExiledFromPaires" )
					set( CPACK_RPM_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_ARCHITECTURE}" )
					set( CPACK_RPM_PACKAGE_GROUP "Development/Tools/Other" )
					set( CPACK_RPM_PACKAGE_LICENSE "MIT" )
					set( CPACK_RPM_SPEC_MORE_DEFINE "%define _rpmdir ${Castor_PACKAGE_DIR}" )
					set( CPACK_RPM_SPEC_MORE_DEFINE "%define _rpmfilename ${CPACK_PACKAGE_FILE_NAME}.rpm" )
					set( CPACK_RPM_SPEC_MORE_DEFINE "%define _unpackaged_files_terminate_build 0" )
					set( CPACK_RPM_SPEC_MORE_DEFINE "%define _topdir ${Castor_PACKAGE_DIR}" )
					include(CPack)
				endif()
			endif ()

			cpack_add_install_type( Full )

			cpack_add_component_group( Main
				EXPANDED
				DISPLAY_NAME "Main"
				DESCRIPTION "Main components to run GratzPaire."
			)
		endif ()

		add_subdirectory( GratzPaire )
	else()
		message( SEND_ERROR "Please select a build type (Debug or Release)" )
	endif()
else ()
	message( SEND_ERROR "Castor3D not found." )
endif ()