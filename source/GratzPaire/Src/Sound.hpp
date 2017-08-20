/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Sound_HPP___
#define ___EFO_Sound_HPP___

#include "GratzPairePrerequisites.hpp"

#include <Design/Signal.hpp>

#if defined( CASTOR_PLATFORM_WINDOWS )
#include <fmod.hpp>
#include <fmod.hpp>
#endif

namespace gratz_paire
{
	bool checkError( FMOD_RESULT result );
	/**
	*\brief
	*	Handles one sound.
	*/
	class Sound
	{
	public:
		using OnEndFunction = std::function< void() >;
		using OnEnd = castor::Signal< OnEndFunction >;
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
		/**
		*\brief
		*	Stops playing the sound.
		*/
		void stop();

	public:
		OnEnd onPlayEnd;

	private:
		static FMOD_RESULT callback( FMOD_CHANNELCONTROL * channelControl
			, FMOD_CHANNELCONTROL_TYPE controlType
			, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
			, void * commandData1
			, void * commandData2 );

	private:
#if defined( CASTOR_PLATFORM_WINDOWS )
		FMOD::Sound * m_sound = nullptr;
		FMOD::Channel * m_channel = nullptr;
		FMOD::System * m_system = nullptr;
#endif
	};
}

#endif
