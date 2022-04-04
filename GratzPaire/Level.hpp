/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Level_HPP___
#define ___EFO_Level_HPP___

#include "Audio.hpp"
#include "Card.hpp"
#include "Hud.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <random>

namespace gratz_paire
{
	/**
	*\brief
	*	Base class for a level.
	*/
	class Level
		: public castor::Named
	{
	public:
		/**
		*\brief
		*	Constructor, initialises the base components.
		*\param[in] scene
		*	The 3D scene.
		*/
		explicit Level( castor3d::Scene & scene
			, castor::String const & name
			, uint32_t cardsCount
			, uint32_t maxErrors
			, std::vector< castor3d::SceneNodeRPtr > const & nodes
			, std::vector< CardObjects > const & cards );
		/**
		*\brief
		*	Initialises the cards for this level.
		*/
		void initialise();
		/**
		*\brief
		*	Checks if the given std::pair is a valid cards pair.
		*/
		bool isPair( castor3d::GeometryRPtr lhs, castor3d::GeometryRPtr rhs );
		/**
		*\brief
		*	Sets the found pairs count, and tells if the level is ended (\p true).
		*/
		bool success();
		/**
		*\brief
		*	Increments the error count, and tells if the game is ended (\p true).
		*/
		bool error();
		/**
		*\return
		*	The error count.
		*/
		inline uint32_t getErrors()const
		{
			return m_errors;
		}
		/**
		*\return
		*	The maximum error count.
		*/
		inline uint32_t getMaxErrors()const
		{
			return m_maxErrors;
		}
		/**
		*\return
		*	The score.
		*/
		inline uint32_t getScore()const
		{
			return m_maxErrors - 1 - m_errors;
		}

	private:
		std::vector< castor3d::SceneNodeRPtr > const & m_allNodes;
		std::vector< castor3d::SceneNodeRPtr > m_nodes;
		std::vector< CardObjects > m_cards;
		uint32_t const m_maxErrors;
		uint32_t const m_maxCards;
		uint32_t m_errors{ 0u };
		uint32_t m_ok{ 0u };
	};
}

#endif
