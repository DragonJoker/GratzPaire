#include "NodeState.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Scene/SceneNode.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace main
	{
		namespace
		{
			float GetValue( Angle const & value )
			{
				return value.degrees();
			}

			float GetValue( float const & value )
			{
				return value;
			}

			template< typename T >
			RangedValue< T > doUpdateVelocity( RangedValue< T > velocity )
			{
				static T const l_zero{};
				auto l_ret = velocity.value() / 1.2f;

				if ( std::abs( GetValue( l_ret ) ) < 0.2f )
				{
					l_ret = l_zero;
				}

				return RangedValue< T >( l_ret, velocity.range() );
			}
		}

		NodeState::NodeState( FrameListener & listener
			, castor3d::SceneNodeSPtr node )
			: m_listener{ listener }
			, m_node{ node }
			, m_originalOrientation{ node->getOrientation() }
			, m_originalPosition{ node->getPosition() }
		{
		}

		void NodeState::reset( float speed )
		{
			SetMaxSpeed( speed );
			m_angularVelocityX = 0.0_degrees;
			m_angularVelocityY = 0.0_degrees;
			m_angles = Angles{ { 0.0_radians, 0.0_radians } };
			m_scalarVelocityX = 0.0f;
			m_scalarVelocityY = 0.0f;
			m_scalarVelocityZ = 0.0f;

			m_listener.postEvent( MakeFunctorEvent( EventType::ePostRender
				, [this]()
				{
					m_node->setOrientation( m_originalOrientation );
					m_node->setPosition( m_originalPosition );
				} ) );
		}

		void NodeState::SetMaxSpeed( float speed )
		{
			m_angularVelocityX.updateRange( makeRange( Angle::fromDegrees( -speed * 2 )
				, Angle::fromDegrees( speed * 2 ) ) );
			m_angularVelocityY.updateRange( makeRange( Angle::fromDegrees( -speed * 2 )
				, Angle::fromDegrees( speed * 2 ) ) );
			m_scalarVelocityX.updateRange( makeRange( -speed, speed ) );
			m_scalarVelocityY.updateRange( makeRange( -speed, speed ) );
			m_scalarVelocityZ.updateRange( makeRange( -speed, speed ) );
		}

		bool NodeState::Update()
		{
			auto angles = m_angles;
			m_angles[0] += m_angularVelocityX.value();
			m_angles[1] += m_angularVelocityY.value();
			m_angularVelocityX = doUpdateVelocity( m_angularVelocityX );
			m_angularVelocityY = doUpdateVelocity( m_angularVelocityY );
			Point3r translate;
			translate[0] = m_scalarVelocityX.value();
			translate[1] = m_scalarVelocityY.value();
			translate[2] = m_scalarVelocityZ.value();
			m_scalarVelocityX = doUpdateVelocity( m_scalarVelocityX );
			m_scalarVelocityY = doUpdateVelocity( m_scalarVelocityY );
			m_scalarVelocityZ = doUpdateVelocity( m_scalarVelocityZ );

			bool result{ angles[0] != m_angles[0] || angles[1] != m_angles[1] };

			if ( translate != Point3r{} )
			{
				auto & orientation = m_node->getOrientation();
				Point3r right{ 1.0_r, 0.0_r, 0.0_r };
				Point3r up{ 0.0_r, 1.0_r, 0.0_r };
				Point3r front{ 0.0_r, 0.0_r, 1.0_r };
				orientation.transform( right, right );
				orientation.transform( up, up );
				orientation.transform( front, front );
				translate = ( right * translate[0] ) + ( up * translate[1] ) + ( front * translate[2] );
				result = true;
			}

			if ( result )
			{
				angles = m_angles;
				m_listener.postEvent( MakeFunctorEvent( EventType::ePostRender
					, [this, translate, angles]()
					{
						m_node->translate( translate );

						Quaternion l_x{ Quaternion::fromAxisAngle( Point3r{ 1.0, 0.0, 0.0 }, angles[0] ) };
						Quaternion l_y{ Quaternion::fromAxisAngle( Point3r{ 0.0, 1.0, 0.0 }, angles[1] ) };
						m_node->setOrientation( m_originalOrientation * l_y * l_x );
					} ) );
			}

			return result;
		}

		void NodeState::SetAngularVelocity( castor::Point2r const & value )noexcept
		{
			m_angularVelocityX = castor::Angle::fromDegrees( value[0] );
			m_angularVelocityY = castor::Angle::fromDegrees( value[1] );
		}

		void NodeState::SetScalarVelocity( castor::Point3r const & value )noexcept
		{
			m_scalarVelocityX = value[0];
			m_scalarVelocityY = value[1];
			m_scalarVelocityZ = value[2];
		}

		void NodeState::addAngularVelocity( castor::Point2r const & value )noexcept
		{
			m_angularVelocityX += castor::Angle::fromDegrees( value[0] );
			m_angularVelocityY += castor::Angle::fromDegrees( value[1] );
		}

		void NodeState::AddScalarVelocity( castor::Point3r const & value )noexcept
		{
			m_scalarVelocityX += value[0];
			m_scalarVelocityY += value[1];
			m_scalarVelocityZ += value[2];
		}
	}
}
