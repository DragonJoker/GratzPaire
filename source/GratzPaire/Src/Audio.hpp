/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Audio_HPP___
#define ___EFO_Audio_HPP___

#include "Sound.hpp"

#include <list>

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
		*	Creates a sound.
		*/
		Sound & addSound( uint32_t id
			, Sound::Type type
			, castor::Path const & file
			, bool looped );
		/**
		*\brief
		*	Plays a sound, given its ID.
		*/
		void playSound( uint32_t id );
#if defined( CASTOR_PLATFORM_WINDOWS )
		/**
		*\return
		*	The FMOD system.
		*/
		FMOD::System * getSystem()const
		{
			return m_system;
		}
		/**
		*\return
		*	The SFX channel.
		*/
		FMOD::Channel * getSfxChannel()const
		{
			return m_sfxChannel;
		}
		/**
		*\return
		*	The music channel.
		*/
		FMOD::Channel * getMusicChannel()const
		{
			return m_ambientChannel;
		}

	private:
		FMOD::System * m_system = nullptr;
		FMOD::Channel * m_ambientChannel = nullptr;
		FMOD::Channel * m_sfxChannel = nullptr;
#endif

	private:
		std::list< Sound > m_sounds;
		std::map< uint32_t, Sound * > m_soundsById;
	};
}

#endif
