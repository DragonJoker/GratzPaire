#include "Game.hpp"

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
	}

	void Game::reset()
	{
		m_state = State::eStarted;
		doPrepareCards();
	}

	void Game::start()
	{
		m_state = State::eStarted;
		m_hud.start();
		m_errors = 0u;
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
			//
			// Update stuff.
			//
		}
	}

	void Game::selectCard( castor3d::GeometrySPtr geometry )
	{
	}

	Point3r Game::convert( castor::Point2i const & position )const
	{
		return Point3r{};
	}

	void Game::doPrepareCards()
	{
	}
}
