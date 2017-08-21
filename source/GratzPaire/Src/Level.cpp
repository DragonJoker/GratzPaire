#include "Level.hpp"

#include <algorithm>
#include <random>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace
	{
		uint32_t doGetIndex( String name )
		{
			name = name.substr( name.find_first_of( '_' ) + 1 );
			name = name.substr( 0, name.find_first_of( '_' ) );
			return uint32_t( string::toInt( name ) );
		}
	}

	Level::Level( Scene & scene
		, String const & name
		, uint32_t cardsCount
		, uint32_t maxErrors
		, std::vector< SceneNodeSPtr > const & nodes
		, std::map< GeometrySPtr, GeometrySPtr > const & cards )
		: Named( name )
		, m_allNodes{ nodes }
		, m_maxErrors{ maxErrors }
		, m_maxCards{ uint32_t( cards.size() / 2 ) }
	{
		auto count = cardsCount / 2;
		for ( size_t i = 1u; i <= count; ++i )
		{
			String card1Name = cuT( "Carte_" ) + string::toString( i );
			String verso1Name = card1Name + cuT( "_Verso" );
			auto itC1 = std::find_if( cards.begin()
				, cards.end()
				, [&verso1Name]( std::pair< GeometrySPtr, GeometrySPtr > const & pair )
				{
					return pair.first->getName() == verso1Name;
				} );
			String card2Name = cuT( "Carte_" ) + string::toString( i + m_maxCards );
			String verso2Name = card2Name + cuT( "_Verso" );
			auto itC2 = std::find_if( cards.begin()
				, cards.end()
				, [&verso2Name]( std::pair< GeometrySPtr, GeometrySPtr > const & pair )
				{
					return pair.first->getName() == verso2Name;
				} );
			REQUIRE( itC1 != cards.end() );
			REQUIRE( itC2 != cards.end() );
			m_cards.emplace( *itC1 );
			m_cards.emplace( *itC2 );
		}

		for ( size_t i = 1u; i <= cardsCount; ++i )
		{
			String nodeName = cuT( "Carte_" ) + string::toString( i );
			auto itN = std::find_if( nodes.begin()
				, nodes.end()
				, [&nodeName]( SceneNodeSPtr node )
			{
				return node->getName() == nodeName;
			} );
			REQUIRE( itN != nodes.end() );
			m_nodes.emplace_back( *itN );
		}
	}

	void Level::initialise()
	{
		m_errors = 0u;
		m_ok = 0u;

		for ( auto & node : m_allNodes )
		{
			node->setVisible( false );
		}

		for ( auto & node : m_nodes )
		{
			node->setOrientation( Quaternion::fromAxisAngle( Point3f{ 0, 0, 0 }, 0.0_degrees ) );
			node->setVisible( true );
		}

		static std::random_device rd;
		std::mt19937 g( rd() );

		std::shuffle( m_nodes.begin(), m_nodes.end(), g );
		auto it = m_nodes.begin();

		for ( auto & card : m_cards )
		{
			card.first->getParent()->attachTo( *it );
			card.second->getParent()->attachTo( *it );
			++it;
		}
	}

	bool Level::isPair( GeometrySPtr lhs, GeometrySPtr rhs )
	{
		auto index1 = doGetIndex( lhs->getName() );
		auto index2 = doGetIndex( rhs->getName() );

		return ( std::max( index2, index1 ) - std::min( index2, index1 ) == m_maxCards );
	}

	bool Level::success()
	{
		m_ok += 2u;
		return m_ok == m_cards.size();
	}

	bool Level::error()
	{
		++m_errors;
		return m_errors >= m_maxErrors;
	}
}
