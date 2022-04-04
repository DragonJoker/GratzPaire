#include "Card.hpp"

#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/AnimatedObject.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>

namespace gratz_paire
{
	static castor::Quaternion const Hidden{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 1, 0, 0 }, 180.0_degrees ) };
	static castor::Quaternion const Revealed{ castor::Quaternion::fromAxisAngle( castor::Point3f{ 1, 0, 0 }, 0.0_degrees ) };
	static castor::Milliseconds const SwipeTime{ 500_ms };
	static castor::Milliseconds const RevealTimeout{ 1000_ms };

	Card::Card( castor3d::GeometryRPtr geometry )
		: m_state{ State::eHidden }
		, m_node{ geometry->getParent()->getParent() }
	{
	}

	bool Card::reveal( castor::Milliseconds const & elapsed )
	{

		if ( isHidden() )
		{
			m_total = 0_ms;
			m_state = State::eRevealing;
		}
		else
		{
			m_total += elapsed;
		}

		bool result = doSwipe( Hidden
				, Revealed
				, SwipeTime );

		if ( result )
		{
			m_state = State::eRevealed;
		}

		return result;
	}

	bool Card::hide( castor::Milliseconds const & elapsed )
	{

		if ( isRevealed() )
		{
			m_total = 0_ms;
			m_state = State::eWaitHide;
		}
		else
		{
			m_total += elapsed;
		}

		bool result = false;

		if ( m_state == State::eWaitHide )
		{
			if ( m_total >= RevealTimeout )
			{
				m_total = 0_ms;
				m_state = State::eHiding;
			}
		}
		else
		{
			result = doSwipe( Revealed
				, Hidden
				, SwipeTime );
		}

		if ( result )
		{
			m_state = State::eHidden;
		}

		return result;
	}

	bool Card::doSwipe( castor::Quaternion const & src
		, castor::Quaternion const & dst
		, castor::Milliseconds const & maxTime )
	{
		auto cur = m_interpolator.interpolate( src, dst, float( m_total.count() ) / float( maxTime.count() ) );
		auto node = m_node;
		m_node->getScene()->getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
			, [node, cur]()
			{
				node->setOrientation( cur );
			} ) );
		return m_total >= maxTime;
	}
}
