#include "Hud.hpp"

#include "Game.hpp"

#include <Cache/OverlayCache.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Overlay/Overlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Scene/Scene.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	Hud::Hud( Game & game
		, Scene const & scene )
		: m_game{ game }
		, m_scene{ scene }
	{
	}

	void Hud::initialise()
	{
		auto userListener = m_scene.getEngine()->getUserInputListener();
		userListener->registerClickAction( cuT( "Menu/Regles" )
			, [this]()
			{
				help();
			} );
		userListener->registerClickAction( cuT( "MenuInGame" )
			, [this]()
			{
				m_game.pause();
			} );
		userListener->registerClickAction( cuT( "Menu/Jouer" )
			, [this]()
			{
				m_game.start();
			} );
		userListener->registerClickAction( cuT( "Menu/Recommencer" )
			, [this]()
			{
				m_game.reset();
				m_game.start();
				update( 0u );
			} );
		userListener->registerClickAction( cuT( "Menu/Continuer" )
			, [this]()
			{
				m_game.resume();
			} );
		userListener->registerClickAction( cuT( "Regles/Retour" )
			, [this]()
			{
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
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( true );
				cache.find( cuT( "Menu" ) )->setVisible( true );
				cache.find( cuT( "MenuInGame" ) )->setVisible( false );
				cache.find( cuT( "Errors" ) )->setVisible( false );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( true );
			} ) );
	}

	void Hud::start()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( false );
				cache.find( cuT( "MenuInGame" ) )->setVisible( true );
				cache.find( cuT( "Errors" ) )->setVisible( true );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::pause()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( true );
				cache.find( cuT( "MenuInGame" ) )->setVisible( false );
				cache.find( cuT( "Errors" ) )->setVisible( true );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( true );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::resume()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( false );
				cache.find( cuT( "MenuInGame" ) )->setVisible( true );
				cache.find( cuT( "Errors" ) )->setVisible( true );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::help()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( false );
				cache.find( cuT( "MenuInGame" ) )->setVisible( false );
				cache.find( cuT( "Errors" ) )->setVisible( false );
				cache.find( cuT( "Regles" ) )->setVisible( true );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::gameOver()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( true );
				cache.find( cuT( "MenuInGame" ) )->setVisible( false );
				cache.find( cuT( "Errors" ) )->setVisible( true );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( false );
				cache.find( cuT( "LoseGame" ) )->setVisible( true );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( true );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::win()
	{
		m_scene.getEngine()->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				auto & cache = m_scene.getEngine()->getOverlayCache();
				cache.find( cuT( "Title" ) )->setVisible( false );
				cache.find( cuT( "Menu" ) )->setVisible( true );
				cache.find( cuT( "MenuInGame" ) )->setVisible( false );
				cache.find( cuT( "Errors" ) )->setVisible( true );
				cache.find( cuT( "Regles" ) )->setVisible( false );
				cache.find( cuT( "WinGame" ) )->setVisible( true );
				cache.find( cuT( "LoseGame" ) )->setVisible( false );
				cache.find( cuT( "Menu/Continuer" ) )->setVisible( false );
				cache.find( cuT( "Menu/Recommencer" ) )->setVisible( true );
				cache.find( cuT( "Menu/Jouer" ) )->setVisible( false );
			} ) );
	}

	void Hud::update( uint32_t errors )
	{
		auto engine = m_scene.getEngine();
		engine->postEvent( MakeFunctorEvent( EventType::ePreRender
			, [engine, errors]()
			{
				auto & cache = engine->getOverlayCache();
				cache.find( cuT( "Errors/Value" ) )->getTextOverlay()->setCaption( string::toString( errors ) );
			} ) );
	}
}
