#include "GratzPaire.hpp"

#include "MainFrame.hpp"

#include <Log/Logger.hpp>
#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>

#include <wx/cmdline.h>
#include <wx/display.h>

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif

wxIMPLEMENT_APP( gratz_paire::main::GratzPaire );

using namespace castor;
using namespace castor3d;

namespace gratz_paire
{
	namespace main
	{
#if defined( NDEBUG )

		static LogType constexpr DefaultLogType = LogType::eInfo;

#else

		static LogType constexpr DefaultLogType = LogType::eDebug;

#endif

		static wxString const ShortName = wxT( "GratzPaire" );
		static wxString const LongName = wxT( "Gratz-Paire" );

		GratzPaire::GratzPaire()
			: wxApp{}
		{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
		}

		bool GratzPaire::OnInit()
		{
#if defined( CASTOR_PLATFORM_WINDOWS ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )

			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif

			bool result = doParseCommandLine();
			wxDisplay display;
			wxRect rect = display.GetClientArea();
			wxWindow * window = nullptr;

			if ( result )
			{
				result = doInitialiseLocale();
			}

			if ( result )
			{
				try
				{
					result = doInitialiseCastor();

					if ( result )
					{
						window = doInitialiseMainFrame();
						result = window != nullptr;
					}
				}
				catch ( Exception & exc )
				{
					Logger::logError( std::stringstream() << ShortName << " - Initialisation failed : " << exc.getFullDescription() );
					result = false;
				}
				catch ( std::exception & exc )
				{
					Logger::logError( std::stringstream() << ShortName << " - Initialisation failed : " << exc.what() );
					result = false;
				}
			}

			wxApp::SetTopWindow( window );

			if ( !result )
			{
				doCleanup();
			}

			return result;
		}

		int GratzPaire::OnExit()
		{
			Logger::logInfo( ShortName.ToStdString() + cuT( " - Exit" ) );
			doCleanup();
			return wxApp::OnExit();
		}

		bool GratzPaire::doParseCommandLine()
		{
			wxCmdLineParser parser( wxApp::argc, wxApp::argv );
			parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
			parser.AddOption( wxT( "l" ), wxT( "log" ), _( "Defines log level" ), wxCMD_LINE_VAL_NUMBER );
			parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
			parser.AddSwitch( wxT( "opengl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
			parser.AddSwitch( wxT( "test" ), wxEmptyString, _( "Defines the renderer to Test" ) );
			bool result = parser.Parse( false ) == 0;

			// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
			if ( !result || parser.Found( wxT( 'h' ) ) )
			{
				parser.Usage();
				result = false;
			}

			if ( result )
			{
				LogType logLevel = LogType::eCount;
				long log;

				if ( !parser.Found( wxT( "l" ), &log ) )
				{
					logLevel = DefaultLogType;
				}
				else
				{
					logLevel = LogType( log );
				}

				Logger::initialise( logLevel );

				if ( parser.Found( wxT( "opengl" ) ) )
				{
					m_rendererType = cuT( "opengl" );
				}

				if ( parser.Found( wxT( "test" ) ) )
				{
					m_rendererType = cuT( "test" );
				}

				wxString l_strFileName;

				if ( parser.GetParamCount() > 0 )
				{
					m_fileName = parser.GetParam( 0 ).ToStdString();
				}
			}

			return result;
		}

		bool GratzPaire::doInitialiseLocale()
		{
			long language = wxLANGUAGE_DEFAULT;
			Path pathCurrent = File::getExecutableDirectory().getPath();

			// load language if possible, fall back to english otherwise
			if ( wxLocale::IsAvailable( language ) )
			{
				m_locale = std::make_unique< wxLocale >( language, wxLOCALE_LOAD_DEFAULT );
				// add locale search paths
				m_locale->AddCatalogLookupPathPrefix( pathCurrent / cuT( "share" ) / ShortName.ToStdString() );
				m_locale->AddCatalog( ShortName );

				if ( !m_locale->IsOk() )
				{
					std::cerr << "Selected language is wrong" << std::endl;
					language = wxLANGUAGE_ENGLISH;
					m_locale = std::make_unique< wxLocale >( language );
				}
			}
			else
			{
				std::cerr << "The selected language is not supported by your system."
					<< "Try installing support for this language." << std::endl;
				language = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( language );
			}

			return true;
		}

		bool GratzPaire::doInitialiseCastor()
		{
			bool result = true;

			if ( !File::directoryExists( Engine::getEngineDirectory() ) )
			{
				File::directoryCreate( Engine::getEngineDirectory() );
			}

			Logger::setFileName( Engine::getEngineDirectory() / ( ShortName.ToStdString() + cuT( ".log" ) ) );
			Logger::logInfo( ShortName.ToStdString() + cuT( " - start" ) );

			m_castor = std::make_unique< Engine >();
			doloadPlugins();

			auto renderers = m_castor->getPluginCache().getPlugins( PluginType::eRenderer );

			if ( renderers.empty() )
			{
				CASTOR_EXCEPTION( "No renderer plug-ins" );
			}
			else if ( renderers.size() == 1 )
			{
				m_rendererType = std::static_pointer_cast< RendererPlugin >( renderers.begin()->second )->getRendererType();
			}

			result = true;

			if ( result )
			{
				result = m_castor->loadRenderer( m_rendererType );
			}

			return result;
		}

		void GratzPaire::doloadPlugins()
		{
			PathArray arrayFiles;
			File::listDirectoryFiles( Engine::getPluginsDirectory(), arrayFiles );
			PathArray arrayKept;

			// Exclude debug plug-in in release builds, and release plug-ins in debug builds
			for ( auto file : arrayFiles )
			{
#if defined( NDEBUG )

				if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

				if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
				{
					arrayKept.push_back( file );
				}
			}

			if ( !arrayKept.empty() )
			{
				PathArray arrayFailed;
				PathArray otherPlugins;

				for ( auto file : arrayKept )
				{
					if ( file.getExtension() == CASTOR_DLL_EXT )
					{
						// Since techniques depend on renderers, we load these first
						if ( file.find( cuT( "RenderSystem" ) ) != String::npos )
						{
							if ( !m_castor->getPluginCache().loadPlugin( file ) )
							{
								arrayFailed.push_back( file );
							}
						}
						else
						{
							otherPlugins.push_back( file );
						}
					}
				}

				// Then we load other plug-ins
				for ( auto file : otherPlugins )
				{
					if ( !m_castor->getPluginCache().loadPlugin( file ) )
					{
						arrayFailed.push_back( file );
					}
				}

				if ( !arrayFailed.empty() )
				{
					Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

					for ( auto file : arrayFailed )
					{
						Logger::logWarning( Path( file ).getFileName() );
					}

					arrayFailed.clear();
				}
			}

			Logger::logInfo( cuT( "Plugins loaded" ) );
		}

		wxWindow * GratzPaire::doInitialiseMainFrame()
		{
			wxAppConsole::SetAppName( ShortName );
			wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

			wxAppConsole::SetAppDisplayName( LongName );
			wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

			m_mainFrame = new MainFrame;

			SetTopWindow( m_mainFrame );
			return m_mainFrame;
		}

		void GratzPaire::doCleanup()
		{
			doCleanupCastor();
			m_locale.reset();
			Logger::cleanup();
			wxImage::CleanUpHandlers();
		}

		void GratzPaire::doCleanupCastor()
		{
			m_castor.reset();
		}
	}
}
