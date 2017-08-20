#include "Audio.hpp"

#include "Game.hpp"

#include <Engine.hpp>

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	Audio::Audio()
	{
#if defined( CASTOR_PLATFORM_WINDOWS )
		auto result = FMOD::System_Create( &m_system );

		if ( checkError( result ) )
		{
			result = m_system->init( 3u, FMOD_INIT_NORMAL, nullptr );

			if ( checkError( result ) )
			{
				result = m_system->getChannel( 0u, &m_sfxChannel );
				checkError( result );
				result = m_system->getChannel( 1u, &m_ambientChannel );
				checkError( result );
				m_ambientChannel->setVolume( 0.3f );
				checkError( result );
			}
		}
#endif
	}

	Audio::~Audio()
	{
		m_sounds.clear();
		m_soundsById.clear();

#if defined( CASTOR_PLATFORM_WINDOWS )
		if ( m_system )
		{
			m_system->close();
			m_system->release();
		}
#endif
	}

	void Audio::update()
	{
		m_system->update();
	}

	Sound & Audio::addSound( uint32_t id
		, Sound::Type type
		, castor::Path const & file
		, bool looped )
	{
		m_sounds.emplace_back( type, file, looped, *this );
		auto & result = m_sounds.back();
		m_soundsById.emplace( id, &result );
		return result;
	}

	void Audio::playSound( uint32_t id )
	{
		auto it = m_soundsById.find( id );
		REQUIRE( it != m_soundsById.end() );
		it->second->play();
	}

	void Audio::stopSound( uint32_t id )
	{
		auto it = m_soundsById.find( id );
		REQUIRE( it != m_soundsById.end() );
		it->second->stop();
	}
}
