/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Sound_HPP___
#define ___EFO_Sound_HPP___

#include "GratzPairePrerequisites.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#include <fmod.hpp>
#endif

namespace gratz_paire
{
	/**
	*\brief
	*	Handles one sound.
	*/
	class Sound
	{
	public:
		enum class Type
		{
			eMusic,
			eSfx
		};

	public:
		/**
		*\brief
		*	Constructor.
		*/
		Sound( Type type
			, castor::Path const & file
			, bool looped
			, Audio const & audio );
		/**
		*\brief
		*	Destructor.
		*/
		~Sound();
		/**
		*\brief
		*	Plays the sound.
		*/
		void play();

	private:
#if defined( CASTOR_PLATFORM_WINDOWS )
		FMOD::Sound * m_sound = nullptr;
		FMOD::Channel * m_channel = nullptr;
		FMOD::System * m_system = nullptr;
#endif
	};
}

#endif
