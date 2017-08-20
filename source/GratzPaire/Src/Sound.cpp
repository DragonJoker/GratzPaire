﻿#include "Sound.hpp"
#include "Audio.hpp"

#include <fmod_errors.h>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	bool checkError( FMOD_RESULT result )
	{
		REQUIRE( result == FMOD_OK );

		if ( result != FMOD_OK )
		{
			std::cerr << FMOD_ErrorString( result ) << std::endl;
		}

		return result == FMOD_OK;
	}

	Sound::Sound( Type type
		, Path const & file
		, bool looped
		, Audio const & audio )
		: m_system{ audio.getSystem() }
		, m_channel{ type == Type::eSfx ? audio.getSfxChannel() : audio.getMusicChannel() }
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		auto loop = ( looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF );
		auto result = audio.getSystem()->createSound( string::stringCast< char >( file ).c_str()
			, type == Type::eSfx
				? FMOD_CREATESAMPLE | loop
				: FMOD_2D | FMOD_CREATESTREAM | loop
			, nullptr
			, &m_sound );
		checkError( result );

		if ( result )
		{
			if ( looped )
			{
				result = m_sound->setLoopCount( -1 );
				checkError( result );
			}
		}
#endif
	}

	Sound::~Sound()
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		if ( m_sound )
		{
			m_sound->release();
		}
#endif
	}

	void Sound::play()
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		if ( m_sound )
		{
			auto result = m_system->playSound( m_sound, nullptr, false, &m_channel );
			m_channel->setUserData( this );
			m_channel->setCallback( Sound::callback );
			checkError( result );
		}
#endif
	}

	void Sound::stop()
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		if ( m_sound )
		{
			auto result = m_channel->stop();
			checkError( result );
		}
#endif
	}

	FMOD_RESULT Sound::callback( FMOD_CHANNELCONTROL * channelControl
		, FMOD_CHANNELCONTROL_TYPE controlType
		, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType
		, void * commandData1
		, void * commandData2 )
	{
		if ( controlType == FMOD_CHANNELCONTROL_CHANNEL )
		{
			auto channel = ( FMOD::Channel * )channelControl;
			Sound * sound;
			channel->getUserData( reinterpret_cast< void ** >( &sound ) );

			if ( sound && callbackType == FMOD_CHANNELCONTROL_CALLBACK_END )
			{
				sound->onPlayEnd();
				channel->setUserData( nullptr );
			}
		}

		return FMOD_OK;
	}
}
