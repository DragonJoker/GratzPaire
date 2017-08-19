#include "GratzPairePrerequisites.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS  ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <gdk/gdkx.h>
#	include <gtk/gtk.h>
#endif

#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Engine.hpp>

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

		castor3d::WindowHandle makeWindowHandle( wxWindow * window )
		{
#if defined( CASTOR_PLATFORM_WINDOWS )

			return castor3d::WindowHandle( std::make_shared< castor3d::IMswWindowHandle >( window->GetHandle() ) );

#elif defined( CASTOR_PLATFORM_LINUX )

			GtkWidget * gtkWidget = static_cast< GtkWidget * >( window->GetHandle() );
			GLXDrawable drawable = 0;
			Display * display = nullptr;

			if ( gtkWidget && gtkWidget->window )
			{
				drawable = GDK_WINDOW_XID( gtkWidget->window );
				GdkDisplay * gtkDisplay = gtk_widget_get_display( gtkWidget );

				if ( gtkDisplay )
				{
					display = gdk_x11_display_get_xdisplay( gtkDisplay );
				}
			}

			return castor3d::WindowHandle( std::make_shared< IXWindowHandle >( drawable, display ) );

#endif
		}
	}
}
