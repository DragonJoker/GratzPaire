/**
See licence file in root folder, MIT.txt
*/
#pragma once

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#ifdef _WIN32
#	include <winsock2.h>
#endif
#include <wx/wx.h>
#include <wx/windowptr.h>
#pragma warning( pop )

#include <Castor3D/Castor3DPrerequisites.hpp>

#include <chrono>
#include <vector>

#include <string>
#include <sstream>
#include <chrono>
#include <memory>
#include <vector>

namespace gratz_paire
{
	// General
	using castor::String;
	using castor::StringStream;
	using castor::Milliseconds;
	using Clock = std::chrono::high_resolution_clock;

	struct Config
	{
		bool validate{ false };
		castor::LogType log{ castor::LogType::eInfo };
		castor::String rendererName;
		uint32_t gpuIndex{ 0u };
	};

	// Game
	class Game;
	class Hud;
	class Audio;

	enum class SoundId
	{
		eMusic,
		eButton,
		eCard,
		eError,
		eWin,
		eGameOver
	};

	// Helpers
	template< typename T >
	inline String toString( T const & value )
	{
		StringStream stream;
		stream << value;
		return stream.str();
	}

	inline String toString( Milliseconds const & value )
	{
		StringStream stream;
		stream << value.count() << " ms";
		return stream.str();
	}

	namespace main
	{
		class GratzPaire;
		class MainFrame;
		class RenderPanel;

		wxString const ApplicationName = wxT( "Gratz-Paire" );

		template< typename T, typename ... Params >
		inline wxWindowPtr< T > wxMakeWindowPtr( Params && ... params )
		{
			return wxWindowPtr< T >( new T( std::forward< Params >( params )... ) );
		}

		castor::Size makeSize( wxSize const & size );
		wxSize makeWxSize( castor::Size const & size );
		wxString makeWxString( castor::String const & value );
		ashes::WindowHandle makeWindowHandle( wxWindow * window );

		static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
		static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
		static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
		static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
		static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
		static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
		static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );
	}
}
