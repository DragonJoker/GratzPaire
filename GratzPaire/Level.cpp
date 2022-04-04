#include "Level.hpp"

#include <algorithm>
#include <random>

namespace gratz_paire
{
	namespace
	{
		uint32_t doGetIndex( String name )
		{
			name = name.substr( name.find_first_of( '_' ) + 1 );
			name = name.substr( 0, name.find_first_of( '_' ) );
			return uint32_t( castor::string::toInt( name ) );
		}
	}

	Level::Level( castor3d::Scene & scene
		, castor::String const & name
		, uint32_t cardsCount
		, uint32_t maxErrors
		, std::vector< castor3d::SceneNodeRPtr > const & nodes
		, std::vector< CardObjects > const & cards )
		: castor::Named( name )
		, m_allNodes{ nodes }
		, m_maxErrors{ maxErrors }
		, m_maxCards{ uint32_t( cards.size() / 2 ) }
	{
		auto count = cardsCount / 2;
		for ( size_t i = 1u; i <= count; ++i )
		{
			auto card1Name = cuT( "Carte_" ) + castor::string::toString( i ) + "_Recto";
			auto itC1 = std::find_if( cards.begin()
				, cards.end()
				, [&card1Name]( CardObjects const & lookup )
				{
					return lookup.recto->getName() == card1Name;
				} );
			auto card2Name = cuT( "Carte_" ) + castor::string::toString( i + m_maxCards ) + "_Recto";
			auto itC2 = std::find_if( cards.begin()
				, cards.end()
				, [&card2Name]( CardObjects const & lookup )
				{
					return lookup.recto->getName() == card2Name;
				} );
			CU_Require( itC1 != cards.end() );
			CU_Require( itC2 != cards.end() );
			m_cards.emplace_back( *itC1 );
			m_cards.emplace_back( *itC2 );
		}

		for ( size_t i = 1u; i <= cardsCount; ++i )
		{
			auto nodeName = cuT( "Carte_" ) + castor::string::toString( i );
			auto itN = std::find_if( m_allNodes.begin()
				, m_allNodes.end()
				, [&nodeName]( castor3d::SceneNodeRPtr node )
			{
				return node->getName() == nodeName;
			} );
			CU_Require( itN != m_allNodes.end() );
			m_nodes.emplace_back( *itN );
		}
	}

	void Level::initialise()
	{
		m_errors = 0u;
		m_ok = 0u;

		for ( auto & node : m_allNodes )
		{
			node->setOrientation( castor::Quaternion::fromAxisAngle( castor::Point3f{ 1, 0, 0 }, 180.0_degrees ) );
			node->setVisible( false );
		}

		for ( auto & node : m_nodes )
		{
			node->setVisible( true );
		}

		std::random_device rd;
		std::mt19937 g( rd() );

		std::shuffle( m_nodes.begin(), m_nodes.end(), g );
		auto it = m_nodes.begin();

		for ( auto & card : m_cards )
		{
			card.recto->getParent()->attachTo( **it );
			card.verso->getParent()->attachTo( **it );
			card.recto->getParent()->setVisible( true );
			card.verso->getParent()->setVisible( true );
			++it;
		}
	}

	bool Level::isPair( castor3d::GeometryRPtr lhs, castor3d::GeometryRPtr rhs )
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
