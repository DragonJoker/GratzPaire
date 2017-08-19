/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include "GratzPairePrerequisites.hpp"
#include "NodeState.hpp"

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
			void doStartTimer( TimerID id );
			void doStopTimer( TimerID id );

			DECLARE_EVENT_TABLE()
			void onSize( wxSizeEvent & p_event );
			void onMove( wxMoveEvent & p_event );
			void onPaint( wxPaintEvent & p_event );
			void onSetFocus( wxFocusEvent & p_event );
			void onKillFocus( wxFocusEvent & p_event );
			void onKeyDown( wxKeyEvent & p_event );
			void onKeyUp( wxKeyEvent & p_event );
			void onMouseLDown( wxMouseEvent & p_event );
			void onMouseLUp( wxMouseEvent & p_event );
			void onMouseRDown( wxMouseEvent & p_event );
			void onMouseRUp( wxMouseEvent & p_event );
			void onMouseMove( wxMouseEvent & p_event );
			void onMouseWheel( wxMouseEvent & p_event );
			void onMouseTimer( wxTimerEvent & p_event );
			void onTimerUp( wxTimerEvent & p_event );
			void onTimerDown( wxTimerEvent & p_event );
			void onTimerLeft( wxTimerEvent & p_event );
			void onTimerRight( wxTimerEvent & p_event );

		private:
			castor::real m_x{ 0.0_r };
			castor::real m_y{ 0.0_r };
			castor::real m_oldX{ 0.0_r };
			castor::real m_oldY{ 0.0_r };
			bool m_mouseLeftDown{ false };
			std::array< wxTimer *, size_t( TimerID::eCount ) > m_timers;
			castor3d::RenderWindowWPtr m_renderWindow;
			castor3d::FrameListenerSPtr m_listener;
			castor3d::GeometryWPtr m_selectedGeometry;
			Game & m_game;
		};
	}
}
