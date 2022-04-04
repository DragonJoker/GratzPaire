/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include "GratzPairePrerequisites.hpp"

#include <Castor3D/Engine.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/panel.h>
#include <wx/timer.h>
#pragma warning( pop )

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
			void reset();
			void setRenderTarget( castor3d::RenderTargetSPtr target );

			castor3d::RenderWindow & getRenderWindow()const
			{
				return *m_renderWindow;
			}

			void disableWindowResize()
			{
				m_resizeWindow = false;
			}

			void enableWindowResize()
			{
				m_resizeWindow = true;
			}

		private:
			float doTransformX( int x );
			float doTransformY( int y );
			int doTransformX( float x );
			int doTransformY( float y );
			void doUpdateSelectedGeometry( castor3d::GeometryRPtr geometry );

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
			float m_x{ 0.0f };
			float m_y{ 0.0f };
			float m_oldX{ 0.0f };
			float m_oldY{ 0.0f };
			bool m_mouseLeftDown{ false };
			bool m_resizeWindow{ true };
			castor3d::RenderWindowUPtr m_renderWindow;
			castor3d::FrameListenerSPtr m_listener;
			castor3d::CameraWPtr m_camera;
			castor3d::SceneRPtr m_scene{};
			Game & m_game;
		};
	}
}
