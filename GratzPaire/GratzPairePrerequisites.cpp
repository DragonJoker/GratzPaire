#include "GratzPairePrerequisites.hpp"

#if defined( CU_PlatformWindows ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <ashespp/Core/PlatformWindowHandle.hpp>
#include <Castor3D/Engine.hpp>

namespace gratz_paire
{
	namespace main
	{
		castor::Size makeSize( wxSize const & size )
		{
			return castor::Size{ uint32_t( size.x ), uint32_t( size.y ) };
		}

		wxSize makeWxSize( castor::Size const & size )
		{
			return wxSize{ int( size.getWidth() ), int( size.getHeight() ) };
		}

		wxString makeWxString( castor::String const & value )
		{
			return wxString( value.c_str(), wxConvUTF8 );
		}

		ashes::WindowHandle makeWindowHandle( wxWindow * window )
		{
#if defined( CU_PlatformWindows )

			return ashes::WindowHandle( std::make_unique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr )
				, window->GetHandle() ) );

#elif defined( CU_PlatformLinux )

			auto gtkWidget = window->GetHandle();

			if ( gtkWidget )
			{
				gtk_widget_realize( gtkWidget );
				auto gdkDisplay = gtk_widget_get_display( gtkWidget );

				if ( gdkDisplay )
				{
#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
#	ifdef GDK_WINDOWING_WAYLAND
					if ( GDK_IS_WAYLAND_DISPLAY( gdkDisplay ) )
					{
						auto gdkWindow = gtk_widget_get_window( gtkWidget );
						auto display = gdk_wayland_display_get_wl_display( gdkDisplay );
						auto surface = gdkWindow
							? gdk_wayland_window_get_wl_surface( gdkWindow )
							: nullptr;
						return ashes::WindowHandle( std::make_unique< ashes::IWaylandWindowHandle >( display, surface ) );
					}
#	endif
#endif
#if defined( VK_USE_PLATFORM_XLIB_KHR )
#	ifdef GDK_WINDOWING_X11
					if ( GDK_IS_X11_DISPLAY( gdkDisplay ) )
					{
						auto gdkWindow = gtk_widget_get_window( gtkWidget );
						auto display = gdk_x11_display_get_xdisplay( gdkDisplay );
						GLXDrawable drawable = gdkWindow
							? gdk_x11_window_get_xid( gdkWindow )
							: 0;
						return ashes::WindowHandle( std::make_unique< ashes::IXWindowHandle >( drawable, display ) );
					}
#	endif
#endif
					CU_Exception( "Unsupported GTK surface type." );
				}
			}

			return ashes::WindowHandle{ nullptr };


#elif defined( CU_PlatformApple )

			auto handle = window->GetHandle();
			makeViewMetalCompatible( handle );
			return ashes::WindowHandle( std::make_unique< ashes::IMacOsWindowHandle >( handle ) );

#else

#	error "Yet unsupported platform"

#endif
		}
	}
}
