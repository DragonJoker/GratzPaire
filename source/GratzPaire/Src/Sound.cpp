#include "Sound.hpp"
#include "Audio.hpp"

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
#if defined( CASTOR_PLATFORM_WINDOWS )
	namespace
	{
		bool checkError( FMOD_RESULT result )
		{
			REQUIRE( result == FMOD_OK );
			return result == FMOD_OK;
		}
	}
#endif

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

		if ( result && looped )
		{
			m_sound->setLoopCount( -1 );
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
			checkError( result );
		}
#endif
	}
}
