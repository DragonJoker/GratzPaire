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
			name = name.substr( 0, name.find_first_of( '_' ) );
			return uint32_t( string::toInt( name ) );
		}
	}

	Game::Game( castor3d::Scene & scene )
		: m_scene{ scene }
		, m_audio{}
		, m_hud{ *this, m_audio, scene }
	{
		m_hud.initialise();
		m_state = State::eInitial;
		doCreateSounds();
		doLoadNodes();
		doLoadCards();
		m_audio.playSound( uint32_t( SoundId::eMusic ) );
		m_levels.emplace_back( m_scene
			, cuT( "Captain obvious" )
			, 4u
			, 3u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Sometimes it hurts" )
			, 4u
			, 2u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Let's start the game" )
			, 6u
			, 3u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Has it started already?" )
			, 6u
			, 2u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Having fun?" )
			, 8u
			, 3u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Try this one!" )
			, 8u
			, 2u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "A bit tougher" )
			, 10u
			, 3u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Niark niark niark!" )
			, 10u
			, 2u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "A bit more!" )
			, 12u
			, 4u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Are you sure you want more?" )
			, 12u
			, 3u
			, m_nodes
			, m_cards );
		m_levels.emplace_back( m_scene
			, cuT( "Go to hell!" )
			, 12u
			, 3u
			, m_nodes
			, m_cards );
	}

	void Game::reset()
	{
		m_state = State::eStarted;
		m_selected.clear();
		m_revealed.clear();
		m_level = 0u;
		m_score = 0u;
	}

	void Game::start()
	{
		m_state = State::eStarted;
		m_hud.start();
		m_saved = Clock::now();
		m_levels[m_level].initialise();
		m_hud.update( m_levels[m_level].getErrors()
			, m_levels[m_level].getMaxErrors() );
		m_hud.update( m_score );
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
		m_audio.update();

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
					doSuccess();
				}
				else
				{
					doError();
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
				m_audio.playSound( uint32_t( SoundId::eCard ) );
			}
		}
	}

	Point3r Game::convert( castor::Point2i const & position )const
	{
		return Point3r{};
	}

	void Game::doCreateSounds()
	{
		Path basePath = Engine::getEngineDirectory() / cuT( "Data" ) / cuT( "Sounds" );
		m_audio.addSound( uint32_t( SoundId::eMusic )
			, Sound::Type::eMusic
			, basePath / cuT( "Ambient.mp3" )
			, true );
		m_audio.addSound( uint32_t( SoundId::eCard )
			, Sound::Type::eSfx
			, basePath / cuT( "Card.mp3" )
			, false );
		m_audio.addSound( uint32_t( SoundId::eButton )
			, Sound::Type::eSfx
			, basePath / cuT( "Button.wav" )
			, false );
		m_audio.addSound( uint32_t( SoundId::eError )
			, Sound::Type::eSfx
			, basePath / cuT( "Error.wav" )
			, false );
		m_audio.addSound( uint32_t( SoundId::eGameOver )
			, Sound::Type::eSfx
			, basePath / cuT( "GameOver.mp3" )
			, false );
		onWinEnd = m_audio.addSound( uint32_t( SoundId::eWin )
			, Sound::Type::eSfx
			, basePath / cuT( "Win.mp3" )
			, false ).onPlayEnd.connect( [this]()
		{
			m_audio.playSound( uint32_t( SoundId::eMusic ) );
		} );
	}

	void Game::doLoadNodes()
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

		for ( auto & node : m_nodes )
		{
			node->setVisible( false );
		}
	}

	void Game::doLoadCards()
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

		for ( auto & pair : m_cards )
		{
			pair.first->getParent()->detach();
			pair.second->getParent()->detach();
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
		REQUIRE( m_revealed.size() == 2u );
		auto it2 = m_revealed.begin();
		auto it1 = it2++;
		return m_levels[m_level].isPair( it1->first, it2->first );
	}

	void Game::doSuccess()
	{
		m_selected.clear();
		m_revealed.clear();

		if ( m_levels[m_level].success() )
		{
			auto score = m_levels[m_level].getScore() * 10;
			++m_level;
			m_score += score * m_level;

			m_audio.stopSound( uint32_t( SoundId::eMusic ) );
			m_audio.playSound( uint32_t( SoundId::eWin ) );

			if ( m_level == m_levels.size() )
			{
				m_state = State::eEnded;
				m_hud.winGame();
			}
			else
			{
				m_hud.winLevel();
				m_hud.update( m_score );
			}
		}
	}

	void Game::doError()
	{
		auto it = m_revealed.begin();
		bool gameOver = false;

		if ( it->second.isRevealed()
			&& ( ++it )->second.isRevealed() )
		{
			m_audio.playSound( uint32_t( SoundId::eCard ) );
			m_audio.playSound( uint32_t( SoundId::eCard ) );
			gameOver = m_levels[m_level].error();
			m_hud.update( m_levels[m_level].getErrors()
				, m_levels[m_level].getMaxErrors() );
			m_audio.playSound( uint32_t( SoundId::eError ) );
		}

		if ( gameOver )
		{
			m_state = State::eEnded;
			m_hud.gameOver();
			m_audio.playSound( uint32_t( SoundId::eGameOver ) );
		}
		else
		{
			doHideCards();
		}
	}
}
