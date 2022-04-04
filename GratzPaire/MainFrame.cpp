#include "MainFrame.hpp"

#include "Game.hpp"
#include "GratzPaire.hpp"
#include "RenderPanel.hpp"

#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <wx/sizer.h>

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
					wxGetApp().getCastor().postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
						, [&game]()
						{
							doUpdate( game );
						} ) );
				}
			}
		}

		MainFrame::MainFrame()
			: wxFrame{ nullptr
				, wxID_ANY
				, ApplicationName
				, wxDefaultPosition
				, MainFrameSize
				, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxMAXIMIZE_BOX }
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
			auto target = doLoadScene( engine
				, castor::File::getExecutableDirectory().getPath() / cuT( "share" ) / cuT( "GratzPaire" ) / cuT( "GratzPaire.zip" ) );

			if ( target )
			{
				m_game = std::make_unique< Game >( *target->getScene() );
				m_panel = wxMakeWindowPtr< RenderPanel >( this, MainFrameSize, *m_game );
				m_panel->setRenderTarget( target );
				auto & window = m_panel->getRenderWindow();
				wxSize size = makeWxSize( window.getSize() );

				if ( window.isFullscreen() )
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

				castor::Logger::logInfo( cuT( "Scene file read" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

				size = GetClientSize();
				SetMinClientSize( size );

#endif

				engine.getRenderLoop().beginRendering();
				engine.postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
					, [this]()
					{
						doUpdate( *m_game );
					} ) );
			}
		}

		castor3d::RenderTargetSPtr MainFrame::doLoadScene( castor3d::Engine & engine
			, castor::Path const & fileName )
		{
			castor3d::RenderTargetSPtr result;

			if ( castor::File::fileExists( fileName ) )
			{
				castor::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

				if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						castor3d::SceneFileParser parser( engine );
						auto preprocessed = parser.processFile( "Castor3D", fileName );

						if ( preprocessed.parse() )
						{
							result = parser.getRenderWindow().renderTarget;
						}
						else
						{
							castor::Logger::logWarning( cuT( "Can't read scene file" ) );
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

				m_panel->reset();

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
