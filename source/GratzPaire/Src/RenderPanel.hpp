/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include "GratzPairePrerequisites.hpp"

#include <Engine.hpp>

#include <wx/panel.h>
#include <wx/timer.h>

namespace gratz_paire
{
	namespace main
	{
		enum class TimerID
		{
			eUp,
			eDown,
			eLeft,
			eRight,
			eMouse,
			eCount,
		};

		class RenderPanel
			: public wxPanel
		{
		public:
			RenderPanel( wxWindow * parent
				, wxSize const & size
				, Game & game );
			~RenderPanel();
			void setRenderWindow( castor3d::RenderWindowSPtr window );

			inline castor3d::RenderWindowSPtr getRenderWindow()const
			{
				return m_renderWindow.lock();
			}

		private:
			castor::real doTransformX( int x );
			castor::real doTransformY( int y );
			int doTransformX( castor::real x );
			int doTransformY( castor::real y );
			void doUpdateSelectedGeometry( castor3d::GeometrySPtr geometry );

			DECLARE_EVENT_TABLE()
			void onSize( wxSizeEvent & p_event );
			void onMove( wxMoveEvent & p_event );
			void onPaint( wxPaintEvent & p_event );
			void onSetFocus( wxFocusEvent & p_event );
			void onKillFocus( wxFocusEvent & p_event );
			void onMouseLDown( wxMouseEvent & p_event );
			void onMouseLUp( wxMouseEvent & p_event );
			void onMouseMove( wxMouseEvent & p_event );

		private:
			castor::real m_x{ 0.0_r };
			castor::real m_y{ 0.0_r };
			castor::real m_oldX{ 0.0_r };
			castor::real m_oldY{ 0.0_r };
			bool m_mouseLeftDown{ false };
			castor3d::RenderWindowWPtr m_renderWindow;
			castor3d::FrameListenerSPtr m_listener;
			Game & m_game;
		};
	}
}
