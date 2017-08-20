#include "RenderPanel.hpp"

#include "GratzPaire.hpp"

#include "Game.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
#include <Miscellaneous/Ray.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Texture/TextureUnit.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace main
	{
		RenderPanel::RenderPanel( wxWindow * parent
			, wxSize const & size
			, Game & game )
			: wxPanel{ parent, wxID_ANY, wxDefaultPosition, size }
			, m_game{ game }
		{
		}

		RenderPanel::~RenderPanel()
		{
		}

		void RenderPanel::setRenderWindow( castor3d::RenderWindowSPtr window )
		{
			m_renderWindow.reset();
			castor::Size sizeWnd{ makeSize( GetClientSize() ) };

			if ( window && window->initialise( sizeWnd, makeWindowHandle( this ) ) )
			{
				castor::Size l_sizeScreen;
				castor::System::getScreenSize( 0, l_sizeScreen );
				GetParent()->SetClientSize( sizeWnd.getWidth(), sizeWnd.getHeight() );
				sizeWnd = makeSize( GetParent()->GetClientSize() );
				GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2
					, std::abs( int( l_sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
				SceneSPtr scene = window->getScene();

				if ( scene )
				{
					m_listener = window->getListener();
					m_renderWindow = window;

					if ( window )
					{
						auto l_lock = makeUniqueLock( scene->getCameraCache() );
						auto l_camera = scene->getCameraCache().begin()->second;
						window->getPickingPass().addScene( *scene, *l_camera );
					}
				}
			}
			else if ( m_listener )
			{
				m_listener.reset();
			}
		}

		real RenderPanel::doTransformX( int x )
		{
			real result = real( x );
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( window )
			{
				result *= real( window->getCamera()->getWidth() ) / GetClientSize().x;
			}

			return result;
		}

		real RenderPanel::doTransformY( int y )
		{
			real result = real( y );
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( window )
			{
				result *= real( window->getCamera()->getHeight() ) / GetClientSize().y;
			}

			return result;
		}

		int RenderPanel::doTransformX( real x )
		{
			int result = int( x );
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( window )
			{
				result = int( x * GetClientSize().x / real( window->getCamera()->getWidth() ) );
			}

			return result;
		}

		int RenderPanel::doTransformY( real y )
		{
			int result = int( y );
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( window )
			{
				result = int( y * GetClientSize().y / real( window->getCamera()->getHeight() ) );
			}

			return result;
		}

		void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometrySPtr newGeometry )
		{
			m_game.selectCard( newGeometry );
		}

		BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
			EVT_SIZE( RenderPanel::onSize )
			EVT_MOVE( RenderPanel::onMove )
			EVT_PAINT( RenderPanel::onPaint )
			EVT_LEFT_DOWN( RenderPanel::onMouseLDown )
			EVT_LEFT_UP( RenderPanel::onMouseLUp )
			EVT_MOTION( RenderPanel::onMouseMove )
		END_EVENT_TABLE()

		void RenderPanel::onSize( wxSizeEvent & p_event )
		{
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( window )
			{
				window->resize( p_event.GetSize().x, p_event.GetSize().y );
			}
			else
			{
				wxClientDC l_dc( this );
				l_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR ) );
				l_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR ) );
				l_dc.DrawRectangle( 0, 0, p_event.GetSize().x, p_event.GetSize().y );
			}

			p_event.Skip();
		}

		void RenderPanel::onMove( wxMoveEvent & p_event )
		{
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( !window )
			{
				wxClientDC l_dc( this );
				l_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR ) );
				l_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR ) );
				l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
			}

			p_event.Skip();
		}

		void RenderPanel::onPaint( wxPaintEvent & p_event )
		{
			RenderWindowSPtr window = m_renderWindow.lock();

			if ( !window )
			{
				wxPaintDC l_dc( this );
				l_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR ) );
				l_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR ) );
				l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
			}

			p_event.Skip();
		}

		void RenderPanel::onSetFocus( wxFocusEvent & p_event )
		{
			p_event.Skip();
		}

		void RenderPanel::onKillFocus( wxFocusEvent & p_event )
		{
			p_event.Skip();
		}

		void RenderPanel::onMouseLDown( wxMouseEvent & p_event )
		{
			m_mouseLeftDown = true;
			auto window = getRenderWindow();

			if ( window )
			{
				if ( m_game.isRunning() )
				{
					auto l_inputListener = wxGetApp().getCastor().getUserInputListener();

					if ( !l_inputListener || !l_inputListener->fireMouseButtonPushed( MouseButton::eLeft ) )
					{
						m_x = doTransformX( p_event.GetX() );
						m_y = doTransformY( p_event.GetY() );
						m_oldX = m_x;
						m_oldY = m_y;
					}
				}
			}
		}

		void RenderPanel::onMouseLUp( wxMouseEvent & p_event )
		{
			m_mouseLeftDown = false;
			auto window = getRenderWindow();

			if ( window )
			{
				auto l_inputListener = wxGetApp().getCastor().getUserInputListener();

				if ( !l_inputListener || !l_inputListener->fireMouseButtonReleased( MouseButton::eLeft ) )
				{
					if ( m_game.isRunning() )
					{
						m_x = doTransformX( p_event.GetX() );
						m_y = doTransformY( p_event.GetY() );
						m_oldX = m_x;
						m_oldY = m_y;
						m_listener->postEvent( MakeFunctorEvent( EventType::ePreRender
							, [this, window]()
							{
								Camera & l_camera = *window->getCamera();
								l_camera.update();
								auto l_type = window->getPickingPass().pick( Position{ int( m_x ), int( m_y ) }, l_camera );

								if ( l_type != PickingPass::NodeType::eNone
									&& l_type != PickingPass::NodeType::eBillboard )
								{
									doUpdateSelectedGeometry( window->getPickingPass().getPickedGeometry() );
								}
							} ) );
					}
				}
			}

			p_event.Skip();
		}

		void RenderPanel::onMouseMove( wxMouseEvent & p_event )
		{
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );
			auto window = getRenderWindow();

			if ( window )
			{
				auto inputListener = wxGetApp().getCastor().getUserInputListener();

				if ( inputListener )
				{
					inputListener->fireMouseMove( Position{ int( m_x ), int( m_y ) } );
				}
			}

			m_oldX = m_x;
			m_oldY = m_y;
			p_event.Skip();
		}
	}
}
