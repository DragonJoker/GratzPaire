#include "Audio.hpp"

#include "Game.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace
	{
		bool checkError( FMOD_RESULT result )
		{
			REQUIRE( result == FMOD_OK );
			return result == FMOD_OK;
		}
	}

	Audio::Audio()
	{
		auto result = FMOD::System_Create( &m_system );

		if ( checkError( result ) )
		{
			result = m_system->init( 32, FMOD_INIT_NORMAL, nullptr );

			if ( checkError( result ) )
			{
				Path basePath = Engine::getEngineDirectory() / cuT( "Data" ) / cuT( "Sounds" );
				result = m_system->getChannel( 0u, &m_sfxChannel );
				checkError( result );
				result = m_system->createSound( string::stringCast< char >( basePath / cuT( "Card.mp3" ) ).c_str()
					, FMOD_CREATESAMPLE
					, nullptr
					, &m_cardSwipe );
				checkError( result );
				result = m_system->getChannel( 1u, &m_ambientChannel );
				checkError( result );
				result = m_system->createSound( string::stringCast< char >( basePath / cuT( "Ambient.mp3" ) ).c_str()
					, FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL
					, nullptr
					, &m_ambient );
				checkError( result );
				m_ambient->setLoopCount( -1 );
				result = m_system->playSound( FMOD_CHANNEL_FREE, m_ambient, false, &m_ambientChannel );
				m_ambientChannel->setVolume( 0.3f );
			}
		}
	}

	Audio::~Audio()
	{
		if ( m_ambient )
		{
			m_ambient->release();
		}

		if ( m_cardSwipe )
		{
			m_cardSwipe->release();
		}

		if ( m_system )
		{
			m_system->close();
			m_system->release();
		}
	}

	void Audio::cardSwipe()
	{
		if ( m_cardSwipe )
		{
			auto result = m_system->playSound( FMOD_CHANNEL_FREE, m_cardSwipe, false, nullptr );
			checkError( result );
		}
	}
}
