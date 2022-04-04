#include "Hud.hpp"

#include "Audio.hpp"
#include "Game.hpp"

#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace gratz_paire
{
	Hud::Hud( Game & game
		, Audio & audio
		, castor3d::Scene const & scene )
		: m_game{ game }
		, m_audio{ audio }
		, m_scene{ scene }
	{
	}

	void Hud::initialise()
	{
		auto userListener = m_scene.getEngine()->getUserInputListener();
		userListener->registerClickAction( cuT( "Menu/Regles" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );
				help();
			} );
		userListener->registerClickAction( cuT( "MenuInGame" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );
				m_game.pause();
			} );
		userListener->registerClickAction( cuT( "Menu/Jouer" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );
				m_game.start();
			} );
		userListener->registerClickAction( cuT( "Menu/Recommencer" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );
				m_game.reset();
				m_game.start();
			} );
		userListener->registerClickAction( cuT( "Menu/Continuer" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );

				if ( m_game.isPaused() )
				{
					m_game.resume();
				}
				else
				{
					m_game.start();
				}
			} );
		userListener->registerClickAction( cuT( "Regles/Retour" )
			, [this]()
			{
				m_audio.playSound( uint32_t( SoundId::eButton ) );

				if ( m_game.isPaused() )
				{
					pause();
				}
				else
				{
					mainMenu();
				}
			} );
		mainMenu();
	}

	void Hud::mainMenu()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( true );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( false );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( true );
			} ) );
	}

	void Hud::start()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( false );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( true );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::pause()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( true );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::resume()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( false );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( true );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::help()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( false );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( false );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( true );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::gameOver()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( true );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::winLevel()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( true );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( true );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::winGame()
	{
		m_scene.getEngine()->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu" ) ).lock()->setVisible( true );
				cache.find( cuT( "MenuInGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "GameInfo" ) ).lock()->setVisible( true );
				cache.find( cuT( "Regles" ) ).lock()->setVisible( false );
				cache.find( cuT( "WinGame" ) ).lock()->setVisible( true );
				cache.find( cuT( "LoseGame" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) ).lock()->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) ).lock()->setVisible( true );
				cache.find( cuT( "Menu/Jouer" ) ).lock()->setVisible( false );
			} ) );
	}

	void Hud::update( uint32_t errors
		, uint32_t maxErrors )
	{
		auto engine = m_scene.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [engine, errors, maxErrors]()
			{
				auto & cache = engine->getOverlayCache();
				auto caption = castor::string::toString( errors ) + cuT( " / " ) + castor::string::toString( maxErrors );
				cache.find( cuT( "Errors/Value" ) ).lock()->getTextOverlay()->setCaption( caption );
			} ) );
	}

	void Hud::update( uint32_t score )
	{
		auto engine = m_scene.getEngine();
		engine->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
			, [engine, score]()
			{
				auto & cache = engine->getOverlayCache();
				auto caption = castor::string::toString( score );
				cache.find( cuT( "Score/Value" ) ).lock()->getTextOverlay()->setCaption( caption );
			} ) );
	}
}
