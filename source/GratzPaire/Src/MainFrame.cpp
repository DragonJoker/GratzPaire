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
			static const wxSize MainFrameSize{ 1024, 768 };
			static const bool CASTOR3D_THREADED = true;

			typedef enum eID
			{
				eID_RENDER_TIMER,
			}	eID;

			void doUpdate( Game & game )
			{
				if ( !wxGetApp().getCastor().isCleaned() )
				{
					game.update();
					wxGetApp().getCastor().postEvent( MakeFunctorEvent( EventType::ePostRender, [&game]()
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
				wxGetApp().getCastor().initialise( 120, CASTOR3D_THREADED );
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
				, File::getExecutableDirectory().getPath() / cuT( "share" ) / cuT( "GratzPaire" ) / cuT( "Data.zip" )
				//, Path{ cuT( "Z:\\Projets\\C++\\GratzPaires\\source\\GratzPaire\\Data\\Data" ) } / cuT( "main.cscn" )
				, engine.getRenderLoop().getWantedFps()
				, engine.isThreaded() );

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

				if ( CASTOR3D_THREADED )
				{
					engine.getRenderLoop().beginRendering();
					engine.postEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
					{
						doUpdate( *m_game );
					} ) );
				}
				else
				{
					m_timer = new wxTimer( this, eID_RENDER_TIMER );
					m_timer->Start( 1000 / engine.getRenderLoop().getWantedFps(), true );
				}
			}
		}

		RenderWindowSPtr MainFrame::doLoadScene( Engine & p_engine
			, castor::Path const & p_fileName
			, uint32_t p_wantedFps
			, bool p_threaded )
		{
			RenderWindowSPtr result;

			if ( File::fileExists( p_fileName ) )
			{
				Logger::logInfo( cuT( "Loading scene file : " ) + p_fileName );

				if ( p_fileName.getExtension() == cuT( "cscn" ) || p_fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						SceneFileParser l_parser( p_engine );

						if ( l_parser.parseFile( p_fileName ) )
						{
							result = l_parser.getRenderWindow();
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
				wxMessageBox( _( "Scene file doesn't exist :" ) + wxString( wxT( "\n" ) ) + p_fileName );
			}

			return result;
		}

		BEGIN_EVENT_TABLE( MainFrame, wxFrame )
			EVT_PAINT( MainFrame::onPaint )
			EVT_CLOSE( MainFrame::onClose )
			EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
			EVT_TIMER( eID_RENDER_TIMER, MainFrame::onRenderTimer )
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

		void MainFrame::onRenderTimer( wxTimerEvent & p_event )
		{
			auto & l_castor = wxGetApp().getCastor();

			if ( !l_castor.isCleaned() )
			{
				if ( !l_castor.isThreaded() )
				{
					l_castor.getRenderLoop().renderSyncFrame();
					m_game->update();
					m_timer->Start( 1000 / l_castor.getRenderLoop().getWantedFps(), true );
				}
			}
		}
	}
}
