#include "RenderPanel.hpp"

#include "GratzPaire.hpp"

#include "Game.hpp"

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

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
			m_renderWindow = castor::makeUnique< castor3d::RenderWindow >( cuT( "RenderPanel" )
				, wxGetApp().getCastor()
				, castor::Size{ uint32_t( GetClientSize().x ), uint32_t( GetClientSize().y ) }
				, makeWindowHandle( this ) );
		}

		RenderPanel::~RenderPanel()
		{
			m_renderWindow.reset();
		}

		void RenderPanel::reset()
		{
			m_renderWindow->cleanup();
		}

		void RenderPanel::setRenderTarget( castor3d::RenderTargetSPtr target )
		{
			m_listener = m_renderWindow->getListener();
			m_renderWindow->initialise( target );
			auto scene = target->getScene();

			if ( scene )
			{
				castor::Size sizeWnd{ makeSize( GetClientSize() ) };
				castor::Size l_sizeScreen;
				castor::System::getScreenSize( 0, l_sizeScreen );
				GetParent()->SetClientSize( int( sizeWnd.getWidth() )
					, int( sizeWnd.getHeight() ) );
				sizeWnd = makeSize( GetParent()->GetClientSize() );
				GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2
					, std::abs( int( l_sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
				auto camera = target->getCamera();

				if ( camera )
				{
					auto l_lock = makeUniqueLock( scene->getCameraCache() );
					auto l_camera = scene->getCameraCache().begin()->second;
					m_renderWindow->addPickingScene( *scene );
					m_camera = camera;
				}

				m_scene = scene;
			}
		}

		float RenderPanel::doTransformX( int x )
		{
			float result = float( x );
			auto camera = m_camera.lock();

			if ( camera )
			{
				result *= float( camera->getWidth() ) / float( GetClientSize().x );
			}

			return result;
		}

		float RenderPanel::doTransformY( int y )
		{
			float result = float( y );
			auto camera = m_camera.lock();

			if ( camera )
			{
				result *= float( camera->getHeight() ) / float( GetClientSize().y );
			}

			return result;
		}

		int RenderPanel::doTransformX( float x )
		{
			int result = int( x );
			auto camera = m_camera.lock();

			if ( camera )
			{
				result = int( x * float( GetClientSize().x ) / float( camera->getWidth() ) );
			}

			return result;
		}

		int RenderPanel::doTransformY( float y )
		{
			int result = int( y );
			auto camera = m_camera.lock();

			if ( camera )
			{
				result = int( y * float( GetClientSize().y ) / float( camera->getHeight() ) );
			}

			return result;
		}

		void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometryRPtr newGeometry )
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
			if ( m_resizeWindow && m_renderWindow )
			{
				m_renderWindow->resize( uint32_t( p_event.GetSize().x )
					, uint32_t( p_event.GetSize().y ) );
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
			p_event.Skip();
		}

		void RenderPanel::onPaint( wxPaintEvent & p_event )
		{
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
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;

			if ( auto inputListener = wxGetApp().getCastor().getUserInputListener() )
			{
				inputListener->fireMouseButtonPushed( castor3d::MouseButton::eLeft );
			}

			p_event.Skip();
		}

		void RenderPanel::onMouseLUp( wxMouseEvent & p_event )
		{
			m_mouseLeftDown = false;
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
			auto inputListener = wxGetApp().getCastor().getUserInputListener();

			if ( !inputListener || !inputListener->fireMouseButtonReleased( castor3d::MouseButton::eLeft ) )
			{
				if ( m_game.isRunning() )
				{
					auto x = m_oldX;
					auto y = m_oldY;
					m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
						, [this, x, y]()
						{
							if ( auto camera = m_camera.lock() )
							{
								camera->update();
								auto l_type = m_renderWindow->pick( castor::Position{ int( x ), int( y ) } );

								if ( l_type != castor3d::PickNodeType::eNone
									&& l_type != castor3d::PickNodeType::eBillboard )
								{
									doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry().get() );
								}
							}
						} ) );
				}
			}

			p_event.Skip();
		}

		void RenderPanel::onMouseMove( wxMouseEvent & p_event )
		{
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );

			if ( auto inputListener = wxGetApp().getCastor().getUserInputListener() )
			{
				inputListener->fireMouseMove( castor::Position{ int( m_x ), int( m_y ) } );
			}

			m_oldX = m_x;
			m_oldY = m_y;
			p_event.Skip();
		}
	}
}
