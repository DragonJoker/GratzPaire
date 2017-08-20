/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Hud_HPP___
#define ___EFO_Hud_HPP___

#include "GratzPairePrerequisites.hpp"

namespace gratz_paire
{
	/**
	*\brief
	*	The game HUD.
	*/
	class Hud
	{
	public:
		/**
		*\brief
		*	Constructor.
		*\param[in] p_game
		*	The game instance.
		*/
		Hud( Game & game
			, Audio & audio
			, castor3d::Scene const & scene );
		/**
		*\brief
		*	Sets the HUD to initial state.
		*/
		void initialise();
		/**
		*\brief
		*	Displays the main menu.
		*/
		void mainMenu();
		/**
		*\brief
		*	Shows "game running" HUD.
		*/
		void start();
		/**
		*\brief
		*	Shows "game paused" HUD.
		*/
		void pause();
		/**
		*\brief
		*	Shows "game running" HUD.
		*/
		void resume();
		/**
		*\brief
		*	Shows "help" HUD.
		*/
		void help();
		/**
		*\brief
		*	Shows "game over" HUD.
		*/
		void gameOver();
		/**
		*\brief
		*	Shows "Win" HUD.
		*/
		void win();
		/**
		*\brief
		*	Updates error count.
		*/
		void update( uint32_t errors );

	private:
		Game & m_game;
		Audio & m_audio;
		castor3d::Scene const & m_scene;
	};
}

#endif
