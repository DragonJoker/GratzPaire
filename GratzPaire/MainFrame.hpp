/**
See licence file in root folder, MIT.txt
*/
#pragma once

#include "GratzPairePrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/frame.h>
#include <wx/windowptr.h>
#pragma warning( pop )

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
			castor3d::RenderTargetSPtr doLoadScene( castor3d::Engine & engine
				, castor::Path const & fileName );

			DECLARE_EVENT_TABLE()
			void onPaint( wxPaintEvent & event );
			void onClose( wxCloseEvent & event );
			void onEraseBackground( wxEraseEvent & event );

		private:
			wxWindowPtr< RenderPanel > m_panel;
			std::unique_ptr< Game > m_game;
			wxTimer * m_timer{ nullptr };
		};
	}
}
