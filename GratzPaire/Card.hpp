/**
See licence file in root folder, MIT.txt
*/
#pragma once
#ifndef ___EFO_Card_HPP___
#define ___EFO_Card_HPP___

#include "GratzPairePrerequisites.hpp"

#include <Castor3D/Animation/Interpolator.hpp>

namespace gratz_paire
{
	struct CardObjects
	{
		castor3d::GeometryRPtr recto;
		castor3d::GeometryRPtr verso;
	};
	/**
	*\brief
	*	Handles the card revealing/hiding.
	*/
	class Card
	{
	private:
		enum class State
		{
			eHidden,
			eRevealing,
			eRevealed,
			eWaitHide,
			eHiding,
		};

	public:
		/**
		*\brief
		*	Constructor
		*\param[in] geometry
		*	The geometry for the card.
		*/
		Card( castor3d::GeometryRPtr geometry );
		/**
		*\brief
		*	Reveals the card.
		*\param[in] elapsed
		*	The time elapsed since last call.
		*\return
		*	\p true if the card is fully revealed.
		*/
		bool reveal( Milliseconds const & elapsed );
		/**
		*\brief
		*	Hides the card.
		*\param[in] elapsed
		*	The time elapsed since last call.
		*\return
		*	\p false if the card is fully revealed.
		*/
		bool hide( Milliseconds const & elapsed );
		/**
		*\return
		*	\p true if the card is fully hidden.
		*/
		bool isHidden()
		{
			return m_state == State::eHidden;
		}
		/**
		*\return
		*	\p true if the card is fully revealed.
		*/
		bool isRevealed()
		{
			return m_state == State::eRevealed;
		}
		/**
		*\return
		*	\p true if the card is being revealed.
		*/
		bool isRevealing()
		{
			return m_state == State::eRevealing;
		}
		/**
		*\return
		*	\p true if the card is being hidden.
		*/
		bool isHiding()
		{
			return m_state == State::eHiding;
		}

	private:
		bool doSwipe( castor::Quaternion const & src
			, castor::Quaternion const & dst
			, Milliseconds const & time );

	private:
		State m_state;
		castor3d::SceneNodeRPtr m_node;
		Milliseconds m_total;
		castor3d::InterpolatorT< castor::Quaternion, castor3d::InterpolatorType::eLinear > const m_interpolator;
	};
}

#endif
