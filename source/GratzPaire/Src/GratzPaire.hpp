/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_GratzPaire_HPP___
#define ___EFO_GratzPaire_HPP___

#include "GratzPairePrerequisites.hpp"

#include <wx/app.h>

namespace gratz_paire
{
	namespace main
	{
		class MainFrame;

		class GratzPaire
			: public wxApp
		{
		public:
			GratzPaire();

			inline MainFrame * getMainFrame()const
			{
				return m_mainFrame;
			}

			inline castor3d::Engine const & getCastor()const
			{
				return *m_castor;
			}

			inline castor3d::Engine & getCastor()
			{
				return *m_castor;
			}

		private:
			/**
			*\brief
			*	Initialises the application.
			*\remarks
			*	Parses the command line, loads language file, loads the plug-ins.
			*\return
			*	If false, the application will stop.
			*/
			virtual bool OnInit();
			/**
			*\brief
			*	Cleans up the application.
			*/
			virtual int OnExit();

		private:
			bool doParseCommandLine();
			bool doInitialiseLocale();
			bool doInitialiseCastor();
			void doloadPlugins();
			void doCleanup();
			void doCleanupCastor();
			wxWindow * doInitialiseMainFrame();

		private:
			MainFrame * m_mainFrame{ nullptr };
			std::unique_ptr< castor3d::Engine > m_castor;
			castor::String m_fileName;
			castor::String m_rendererType{ cuT( "opengl" ) };
			std::unique_ptr< wxLocale > m_locale;
		};
	}
}

wxDECLARE_APP( gratz_paire::main::GratzPaire );

#endif
