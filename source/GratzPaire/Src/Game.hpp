﻿/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Game_HPP___
#define ___EFO_Game_HPP___

#include "Audio.hpp"
#include "Card.hpp"
#include "Hud.hpp"
#include "Level.hpp"

namespace gratz_paire
{
	/**
	*\brief
	*	Main class for the game logic.
	*/
	class Game
	{
	private:
		enum class State
		{
			eInitial,
			eStarted,
			ePaused,
			eEnded,
		};

	public:
		/**
		*\brief
		*	Constructor, initialises the base components.
		*\param[in] scene
		*	The 3D scene.
		*/
		explicit Game( castor3d::Scene & scene );
		/**
		*\brief
		*	Resets the game so a new game can be played.
		*/
		void reset();
		/**
		*\brief
		*	Starts the game.
		*/
		void start();
		/**
		*\brief
		*	Pauses the game.
		*/
		void pause();
		/**
		*\brief
		*	Resumes to the game from a pause.
		*/
		void resume();
		/**
		*\brief
		*	Displays help.
		*/
		void help();
		/**
		*\brief
		*	Game loop function.
		*/
		void update();
		/**
		*\brief
		*	Selects the given card.
		*\param[in] geometry
		*	The geometry.
		*/
		void selectCard( castor3d::GeometrySPtr geometry );
		/**
		*\brief
		*	Converts a cell position to 3D position.
		*\param[in] position
		*	The cell position.
		*\return
		*	The 3D position
		*/
		castor::Point3r convert( castor::Point2i const & position )const;
		/**
		*\return
		*	\p true if the game is started.
		*/
		inline bool isStarted()const
		{
			return m_state >= State::eStarted && m_state < State::eEnded;
		}
		/**
		*\return
		*	\p true if the game is started and running.
		*/
		inline bool isRunning()const
		{
			return m_state == State::eStarted;
		}
		/**
		*\return
		*	\p true if the game is ended.
		*/
		inline bool isEnded()const
		{
			return m_state == State::eEnded;
		}
		/**
		*\return
		*	\p true if the game is paused.
		*/
		inline bool isPaused()const
		{
			return m_state == State::ePaused;
		}

	private:
		void doCreateSounds();
		void doLoadNodes();
		void doLoadCards();
		void doRevealCards();
		void doHideCards();
		bool doIsPair();
		void doSuccess();
		void doError();

	private:
		// Persistent data.
		castor3d::Scene & m_scene;
		std::vector< castor3d::SceneNodeSPtr > m_nodes;
		// key => verso, value => recto.
		std::map< castor3d::GeometrySPtr, castor3d::GeometrySPtr > m_cards;
		Audio m_audio;
		Hud m_hud;
		Sound::OnEnd::connection onWinEnd;
		// Varying data.
		Clock::time_point m_saved;
		castor::Milliseconds m_elapsed;
		std::atomic< State > m_state;
		std::map< castor3d::GeometrySPtr, Card > m_selected;
		std::map< castor3d::GeometrySPtr, Card > m_revealed;
		std::vector< Level > m_levels;
		uint32_t m_level{ 0u };
		uint32_t m_score{ 0u };
	};
}

#endif
