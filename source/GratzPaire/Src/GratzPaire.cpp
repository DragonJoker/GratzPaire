#include "GratzPaire.hpp"

#include "MainFrame.hpp"

#include <Log/Logger.hpp>
#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>

#include <wx/cmdline.h>
#include <wx/display.h>

wxIMPLEMENT_APP( gratz_paire::main::GratzPaire );

using namespace castor;
using namespace castor3d;

#if defined( NDEBUG )

static const LogType ELogType_DEFAULT = LogType::eInfo;

#else

static const LogType ELogType_DEFAULT = LogType::eDebug;

#endif

namespace gratz_paire
{
	namespace main
	{
		static wxString const ShortName = wxT( "GratzPaire" );
		static wxString const LongName = wxT( "Gratz-Paire" );

		GratzPaire::GratzPaire()
			: wxApp{}
		{
		}

		bool GratzPaire::OnInit()
		{
#if defined( CASTOR_PLATFORM_WINDOWS ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )

			_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif

			bool result = doParseCommandLine();
			wxDisplay l_display;
			wxRect l_rect = l_display.GetClientArea();
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
			wxCmdLineParser l_parser( wxApp::argc, wxApp::argv );
			l_parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
			l_parser.AddOption( wxT( "l" ), wxT( "log" ), _( "Defines log level" ), wxCMD_LINE_VAL_NUMBER );
			l_parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
			l_parser.AddSwitch( wxT( "opengl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
			l_parser.AddSwitch( wxT( "test" ), wxEmptyString, _( "Defines the renderer to Test" ) );
			bool result = l_parser.Parse( false ) == 0;

			// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
			if ( !result || l_parser.Found( wxT( 'h' ) ) )
			{
				l_parser.Usage();
				result = false;
			}

			if ( result )
			{
				LogType l_eLogLevel = LogType::eCount;
				long l_log;

				if ( !l_parser.Found( wxT( "l" ), &l_log ) )
				{
					l_eLogLevel = ELogType_DEFAULT;
				}
				else
				{
					l_eLogLevel = LogType( l_log );
				}

				Logger::initialise( l_eLogLevel );

				if ( l_parser.Found( wxT( "opengl" ) ) )
				{
					m_rendererType = cuT( "opengl" );
				}

				if ( l_parser.Found( wxT( "test" ) ) )
				{
					m_rendererType = cuT( "test" );
				}

				wxString l_strFileName;

				if ( l_parser.GetParamCount() > 0 )
				{
					m_fileName = l_parser.GetParam( 0 ).ToStdString();
				}
			}

			return result;
		}

		bool GratzPaire::doInitialiseLocale()
		{
			long l_lLanguage = wxLANGUAGE_DEFAULT;
			Path l_pathCurrent = File::getExecutableDirectory().getPath();

			// load language if possible, fall back to english otherwise
			if ( wxLocale::IsAvailable( l_lLanguage ) )
			{
				m_locale = std::make_unique< wxLocale >( l_lLanguage, wxLOCALE_LOAD_DEFAULT );
				// add locale search paths
				m_locale->AddCatalogLookupPathPrefix( l_pathCurrent / cuT( "share" ) / ShortName.ToStdString() );
				m_locale->AddCatalog( ShortName );

				if ( !m_locale->IsOk() )
				{
					std::cerr << "Selected language is wrong" << std::endl;
					l_lLanguage = wxLANGUAGE_ENGLISH;
					m_locale = std::make_unique< wxLocale >( l_lLanguage );
				}
			}
			else
			{
				std::cerr << "The selected language is not supported by your system."
					<< "Try installing support for this language." << std::endl;
				l_lLanguage = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( l_lLanguage );
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

			auto l_renderers = m_castor->getPluginCache().getPlugins( PluginType::eRenderer );

			if ( l_renderers.empty() )
			{
				CASTOR_EXCEPTION( "No renderer plug-ins" );
			}
			else if ( l_renderers.size() == 1 )
			{
				m_rendererType = std::static_pointer_cast< RendererPlugin >( l_renderers.begin()->second )->getRendererType();
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
			PathArray l_arrayFiles;
			File::listDirectoryFiles( Engine::getPluginsDirectory(), l_arrayFiles );
			PathArray l_arrayKept;

			// Exclude debug plug-in in release builds, and release plug-ins in debug builds
			for ( auto l_file : l_arrayFiles )
			{
#if defined( NDEBUG )

				if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

				if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
				{
					l_arrayKept.push_back( l_file );
				}
			}

			if ( !l_arrayKept.empty() )
			{
				PathArray l_arrayFailed;
				PathArray l_otherPlugins;

				for ( auto l_file : l_arrayKept )
				{
					if ( l_file.getExtension() == CASTOR_DLL_EXT )
					{
						// Since techniques depend on renderers, we load these first
						if ( l_file.find( cuT( "RenderSystem" ) ) != String::npos )
						{
							if ( !m_castor->getPluginCache().loadPlugin( l_file ) )
							{
								l_arrayFailed.push_back( l_file );
							}
						}
						else
						{
							l_otherPlugins.push_back( l_file );
						}
					}
				}

				// Then we load other plug-ins
				for ( auto l_file : l_otherPlugins )
				{
					if ( !m_castor->getPluginCache().loadPlugin( l_file ) )
					{
						l_arrayFailed.push_back( l_file );
					}
				}

				if ( !l_arrayFailed.empty() )
				{
					Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

					for ( auto l_file : l_arrayFailed )
					{
						Logger::logWarning( Path( l_file ).getFileName() );
					}

					l_arrayFailed.clear();
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
