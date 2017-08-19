/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Audio_HPP___
#define ___EFO_Audio_HPP___

#include "GratzPairePrerequisites.hpp"

#include <fmod.hpp>

namespace gratz_paire
{
	/**
	*\brief
	*	The game audio handler.
	*/
	class Audio
	{
	public:
		/**
		*\brief
		*	Constructor.
		*/
		Audio();
		/**
		*\brief
		*	Destructor.
		*/
		~Audio();
		/**
		*\brief
		*	Plays the card swipe sound.
		*/
		void cardSwipe();

	private:
		FMOD::System * m_system = nullptr;
		FMOD::Sound * m_ambient = nullptr;
		FMOD::Channel * m_ambientChannel = nullptr;
		FMOD::Sound * m_cardSwipe = nullptr;
		FMOD::Channel * m_sfxChannel = nullptr;
	};
}

#endif
