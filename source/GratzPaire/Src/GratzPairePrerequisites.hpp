/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include <Castor3DPrerequisites.hpp>

#include <wx/wx.h>
#include <wx/windowptr.h>

#include <chrono>
#include <vector>

#include <Castor3DPrerequisites.hpp>

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

	// Game
	class Game;

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
		castor3d::WindowHandle makeWindowHandle( wxWindow * window );

		static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
		static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
		static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
		static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
		static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
		static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
		static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );
	}
}
