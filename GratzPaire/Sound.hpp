/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Sound_HPP___
#define ___EFO_Sound_HPP___

#include "GratzPairePrerequisites.hpp"

#include <CastorUtils/Design/Signal.hpp>

#if defined( CU_PlatformWindows )
#include <fmod.hpp>
#include <fmod.hpp>
#endif

namespace gratz_paire
{
#if defined( CU_PlatformWindows )
	bool checkError( FMOD_RESULT result );
#endif
	/**
	*\brief
	*	Handles one sound.
	*/
	class Sound
	{
	public:
		using OnEndFunction = std::function< void() >;
		using OnEnd = castor::SignalT< OnEndFunction >;
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

#if defined( CU_PlatformWindows )
	private:
		static FMOD_RESULT callback( FMOD_CHANNELCONTROL * channelControl
			, FMOD_CHANNELCONTROL_TYPE controlType
			, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
			, void * commandData1
			, void * commandData2 );

	private:
		FMOD::System * m_system = nullptr;
		FMOD::Channel * m_channel = nullptr;
		FMOD::Sound * m_sound = nullptr;
#endif
	};
}

#endif
