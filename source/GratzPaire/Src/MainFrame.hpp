/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include "GratzPairePrerequisites.hpp"

#include <wx/frame.h>
#include <wx/windowptr.h>

namespace gratz_paire
{
	namespace main
	{
		class MainFrame
			: public wxFrame
		{
		public:
			MainFrame();
			~MainFrame();

		private:
			void doLoadScene();
			castor3d::RenderWindowSPtr doLoadScene( castor3d::Engine & engine
				, castor::Path const & fileName
				, uint32_t wantedFps
				, bool threaded );

			DECLARE_EVENT_TABLE()
			void onPaint( wxPaintEvent & event );
			void onClose( wxCloseEvent & event );
			void onEraseBackground( wxEraseEvent & event );
			void onRenderTimer( wxTimerEvent & event );

		private:
			wxWindowPtr< RenderPanel > m_panel;
			std::unique_ptr< Game > m_game;
			wxTimer * m_timer{ nullptr };
		};
	}
}
