#include "Game.hpp"

#include <Animation/Interpolator.hpp>
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

#include <algorithm>
#include <random>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace
	{
		static uint32_t const MaxErrors = 4u;

		uint32_t doGetIndex( String name )
		{
			name = name.substr( name.find_first_of( '_' ) + 1 );
			std::cout << name << std::endl;
			name = name.substr( 0, name.find_first_of( '_' ) );
			std::cout << name << std::endl;
			return uint32_t( string::toInt( name ) );
		}
	}

	Game::Game( castor3d::Scene & scene )
		: m_scene{ scene }
		, m_hud{ *this, scene }
	{
		m_hud.initialise();
		m_state = State::eInitial;

		{
			auto & nodes = m_scene.getSceneNodeCache();
			auto lock = makeUniqueLock( nodes );
			m_nodes.push_back( nodes.find( cuT( "Carte_1" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_2" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_3" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_4" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_5" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_6" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_7" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_8" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_9" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_10" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_11" ) ) );
			m_nodes.push_back( nodes.find( cuT( "Carte_12" ) ) );
		}

		{
			auto & geometries = m_scene.getGeometryCache();
			auto lock = makeUniqueLock( geometries );
			m_cards.emplace( geometries.find( cuT( "Carte_1_Verso" ) ), geometries.find( cuT( "Carte_1_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_2_Verso" ) ), geometries.find( cuT( "Carte_2_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_3_Verso" ) ), geometries.find( cuT( "Carte_3_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_4_Verso" ) ), geometries.find( cuT( "Carte_4_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_5_Verso" ) ), geometries.find( cuT( "Carte_5_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_6_Verso" ) ), geometries.find( cuT( "Carte_6_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_7_Verso" ) ), geometries.find( cuT( "Carte_7_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_8_Verso" ) ), geometries.find( cuT( "Carte_8_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_9_Verso" ) ), geometries.find( cuT( "Carte_9_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_10_Verso" ) ), geometries.find( cuT( "Carte_10_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_11_Verso" ) ), geometries.find( cuT( "Carte_11_Recto" ) ) );
			m_cards.emplace( geometries.find( cuT( "Carte_12_Verso" ) ), geometries.find( cuT( "Carte_12_Recto" ) ) );
		}
	}

	void Game::reset()
	{
		m_state = State::eStarted;
		m_selected.clear();
		m_revealed.clear();
		doPrepareCards();
	}

	void Game::start()
	{
		m_state = State::eStarted;
		m_hud.start();
		m_saved = Clock::now();
		m_errors = 0u;
		m_ok = 0u;
		doPrepareCards();
	}

	void Game::pause()
	{
		m_state = State::ePaused;
		m_hud.pause();
	}

	void Game::resume()
	{
		m_state = State::eStarted;
		m_hud.resume();
		m_saved = Clock::now();
	}

	void Game::help()
	{
		m_state = State::ePaused;
		m_hud.help();
	}

	void Game::update()
	{
		if ( isRunning() )
		{
#if !defined( NDEBUG )
			m_elapsed = Milliseconds{ 40 };
#else
			m_elapsed = std::chrono::duration_cast< Milliseconds >( Clock::now() - m_saved );
#endif
			doRevealCards();

			if ( m_revealed.size() == 2u )
			{
				if ( doIsPair() )
				{
					m_selected.clear();
					m_revealed.clear();
					m_ok += 2u;

					if ( m_ok == m_cards.size() )
					{
						m_state = State::eEnded;
						m_hud.win();
					}
				}
				else
				{
					auto it = m_revealed.begin();
					if ( it->second.isRevealed()
						&& ( ++it )->second.isRevealed() )
					{
						++m_errors;
						m_hud.update( m_errors );
					}

					if ( m_errors >= MaxErrors )
					{
						m_state = State::eEnded;
						m_hud.gameOver();
					}
					else
					{
						doHideCards();
					}
				}
			}

			m_saved = Clock::now();
		}
	}

	void Game::selectCard( GeometrySPtr geometry )
	{
		if ( geometry->getName().find( "_Verso" ) != String::npos )
		{
			auto itC = m_cards.find( geometry );
			auto itS = m_selected.find( geometry );

			if ( itC != m_cards.end()
				&& itS == m_selected.end()
				&& m_selected.size() < 2u
				&& m_revealed.size() < 2u )
			{
				m_selected.emplace( geometry, Card{ geometry } );
			}
		}
	}

	Point3r Game::convert( castor::Point2i const & position )const
	{
		return Point3r{};
	}

	void Game::doPrepareCards()
	{
		for ( auto & node : m_nodes )
		{
			node->setOrientation( Quaternion::fromAxisAngle( Point3f{ 0, 0, 0 }, 0.0_degrees ) );
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

	void Game::doRevealCards()
	{
		for ( auto & card : m_selected )
		{
			if ( card.second.reveal( m_elapsed ) )
			{
				m_revealed.emplace( card );
			}
		}
	}

	void Game::doHideCards()
	{
		if ( m_revealed.size() == 2u )
		{
			uint32_t hidden = 0u;

			for ( auto & card : m_revealed )
			{
				hidden += card.second.hide( m_elapsed )
					? 1u
					: 0u;
			}

			if ( hidden == m_revealed.size() )
			{
				m_revealed.clear();
				m_selected.clear();
			}
		}
	}

	bool Game::doIsPair()
	{
		auto it = m_revealed.begin();
		auto index1 = doGetIndex( it->first->getName() );
		++it;
		auto index2 = doGetIndex( it->first->getName() );

		return ( std::max( index2, index1 ) - std::min( index2, index1 ) == m_cards.size() / 2u );
	}
}
