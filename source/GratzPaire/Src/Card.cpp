#include "Card.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Animation/AnimatedObject.hpp>
#include <Scene/Animation/AnimatedObjectGroup.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/PointLight.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	Card::Card( GeometrySPtr geometry )
		: m_state{ State::eHidden }
		, m_node{ geometry->getParent()->getParent() }
	{
	}

	bool Card::reveal( Milliseconds const & elapsed )
	{
		static Quaternion const src{ Quaternion::fromAxisAngle( Point3r{ 0, 0, 0 }, 0.0_degrees ) };
		static Quaternion const dst{ Quaternion::fromAxisAngle( Point3r{ 1, 0, 0 }, 180.0_degrees ) };
		static Milliseconds const time{ 500_ms };

		if ( isHidden() )
		{
			m_total = 0_ms;
			m_state = State::eRevealing;
		}
		else
		{
			m_total += elapsed;
		}

		bool result = doSwipe( src
				, dst
				, time );

		if ( result )
		{
			m_state = State::eRevealed;
		}

		return result;
	}

	bool Card::hide( Milliseconds const & elapsed )
	{
		static Quaternion const src{ Quaternion::fromAxisAngle( Point3r{ 1, 0, 0 }, 180.0_degrees ) };
		static Quaternion const dst{ Quaternion::fromAxisAngle( Point3r{ 0, 0, 0 }, 0.0_degrees ) };
		static Milliseconds const time{ 500_ms };
		static Milliseconds const waitTime{ 1000_ms };

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
			if ( m_total >= waitTime )
			{
				m_total = 0_ms;
				m_state = State::eHiding;
			}
		}
		else
		{
			result = doSwipe( src
				, dst
				, time );
		}

		if ( result )
		{
			m_state = State::eHidden;
		}

		return result;
	}

	bool Card::doSwipe( Quaternion const & src
		, Quaternion const & dst
		, Milliseconds const & maxTime )
	{
		auto cur = m_interpolator.interpolate( src, dst, float( m_total.count() ) / float( maxTime.count() ) );
		auto node = m_node;
		m_node->getScene()->getEngine()->postEvent( makeFunctorEvent( EventType::ePostRender
			, [node, cur]()
			{
				node->setOrientation( cur );
			} ) );
		return m_total >= maxTime;
	}
}
