#include "MainFrame.hpp"

#include "Game.hpp"
#include "GratzPaire.hpp"
#include "RenderPanel.hpp"

#include <Event/Frame/FunctorEvent.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/SceneFileParser.hpp>

#include <wx/sizer.h>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace main
	{
		namespace
		{
			static wxSize const MainFrameSize{ 1024, 768 };

			void doUpdate( Game & game )
			{
				if ( !wxGetApp().getCastor().isCleaned() )
				{
					game.update();
					wxGetApp().getCastor().postEvent( makeFunctorEvent( EventType::ePostRender
						, [&game]()
						{
							doUpdate( game );
						} ) );
				}
			}
		}

		MainFrame::MainFrame()
			: wxFrame{ nullptr, wxID_ANY, ApplicationName, wxDefaultPosition, MainFrameSize, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxMAXIMIZE_BOX }
		{
			SetClientSize( MainFrameSize );
			Show( true );

			try
			{
				wxGetApp().getCastor().initialise( 120, true );
				doLoadScene();
				wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
				sizer->Add( m_panel.get(), wxSizerFlags( 1 ).Shaped().Centre() );
				sizer->SetSizeHints( this );
				SetSizer( sizer );
			}
			catch ( std::exception & p_exc )
			{
				wxMessageBox( p_exc.what() );
			}
		}

		MainFrame::~MainFrame()
		{
		}

		void MainFrame::doLoadScene()
		{
			auto & engine = wxGetApp().getCastor();
			auto window = doLoadScene( engine
				, File::getExecutableDirectory().getPath() / cuT( "share" ) / cuT( "GratzPaire" ) / cuT( "Data.zip" ) );

			if ( window )
			{
				m_game = std::make_unique< Game >( *window->getScene() );
				m_panel = wxMakeWindowPtr< RenderPanel >( this, MainFrameSize, *m_game );
				m_panel->setRenderWindow( window );
				wxSize size = makeWxSize( window->getRenderTarget()->getSize() );

				if ( window->isInitialised() )
				{
					if ( window->isFullscreen() )
					{
						ShowFullScreen( true, wxFULLSCREEN_ALL );
					}

					if ( !IsMaximized() )
					{
						SetClientSize( size );
					}
					else
					{
						Maximize( false );
						SetClientSize( size );
						Maximize();
					}

					Logger::logInfo( cuT( "Scene file read" ) );
				}
				else
				{
					throw std::runtime_error{ "Impossible d'initialiser la fenêtre de rendu." };
				}

#if wxCHECK_VERSION( 2, 9, 0 )

				SetMinClientSize( size );

#endif

				engine.getRenderLoop().beginRendering();
				engine.postEvent( makeFunctorEvent( EventType::ePostRender
					, [this]()
					{
						doUpdate( *m_game );
					} ) );
			}
		}

		RenderWindowSPtr MainFrame::doLoadScene( Engine & engine
			, castor::Path const & fileName )
		{
			RenderWindowSPtr result;

			if ( File::fileExists( fileName ) )
			{
				Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

				if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						SceneFileParser parser( engine );

						if ( parser.parseFile( fileName ) )
						{
							result = parser.getRenderWindow();
						}
						else
						{
							Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						wxMessageBox( _( "Failed to parse the scene file, with following error:" ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ) );
					}
				}
			}
			else
			{
				wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + fileName );
			}

			return result;
		}

		BEGIN_EVENT_TABLE( MainFrame, wxFrame )
			EVT_PAINT( MainFrame::onPaint )
			EVT_CLOSE( MainFrame::onClose )
			EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
		END_EVENT_TABLE()

		void MainFrame::onPaint( wxPaintEvent & p_event )
		{
			wxPaintDC paintDC( this );
			p_event.Skip();
		}

		void MainFrame::onClose( wxCloseEvent & p_event )
		{
			auto & engine = wxGetApp().getCastor();
			Hide();

			if ( m_timer )
			{
				m_timer->Stop();
				delete m_timer;
				m_timer = nullptr;
			}

			if ( m_panel )
			{
				if ( engine.isThreaded() )
				{
					engine.getRenderLoop().pause();
				}

				m_panel->setRenderWindow( nullptr );

				if ( engine.isThreaded() )
				{
					engine.getRenderLoop().resume();
				}
			}

			engine.cleanup();

			if ( m_panel )
			{
				m_panel->Close( true );
				m_panel = nullptr;
			}

			DestroyChildren();
			p_event.Skip();
		}

		void MainFrame::onEraseBackground( wxEraseEvent & p_event )
		{
			p_event.Skip();
		}
	}
}
