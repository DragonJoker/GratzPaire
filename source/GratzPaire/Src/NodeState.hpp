/**
See licence file in root folder, MIT.txt
*/
#ifndef ___EFO_NodeState_HPP___
#define ___EFO_NodeState_HPP___
#pragma once

#include <Castor3DPrerequisites.hpp>
#include <Math/Angle.hpp>
#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>
#include <Math/RangedValue.hpp>

namespace gratz_paire
{
	namespace main
	{
		using Angles = std::array< castor::Angle, 2u >;
		/**
		*\brief
		*	Classe de gestion des déplacements d'un noeud de scène.
		*/
		class NodeState
		{
		public:
			/**
			*\brief
			*	Définit le noeud affecté par les évènements.
			*\param[in] p_listener
			*	Le listener qui recevra les évènements.
			*\param[in] p_node
			*	Le noeud
			*/
			NodeState( castor3d::FrameListener & listener
				, castor3d::SceneNodeSPtr node );
			/**
			*\brief
			*	Réinitialise l'état.
			*/
			void reset( float speed );
			/**
			*\brief
			*	Met à jour la vitesse maximale (de rotation et translation).
			*/
			void SetMaxSpeed( float speed );
			/**
			*\brief
			*	Met à jour l'angle et le zoom en fonction des vitesses.
			*\return
			 * 	\p true s'il y a eu du mouvement.
			*/
			bool Update();
			/**
			*\brief
			*	Définit la vitesse de rotation du noeud.
			*param[in] value
			*	La nouvelle valeur.
			*/
			void SetAngularVelocity( castor::Point2r const & value )noexcept;
			/**
			*\brief
			*	Définit la vitesse de translation du noeud.
			*param[in] value
			*	La nouvelle valeur.
			*/
			void SetScalarVelocity( castor::Point3r const & value )noexcept;
			/**
			*\brief
			*	Définit la vitesse de rotation du noeud.
			*param[in] value
			*	La nouvelle valeur.
			*/
			void addAngularVelocity( castor::Point2r const & value )noexcept;
			/**
			*\brief
			*	Définit la vitesse de translation du noeud.
			*param[in] value
			*	La nouvelle valeur.
			*/
			void AddScalarVelocity( castor::Point3r const & value )noexcept;

		private:
			//! Le listener qui recevra les évènements de déplacement / rotation.
			castor3d::FrameListener & m_listener;
			//! Le noeud de scène affecté par les évènements.
			castor3d::SceneNodeSPtr const m_node;
			//! La position originelle du noeud.
			castor::Point3r const m_originalPosition;
			//! L'orientation originelle du noeud.
			castor::Quaternion const m_originalOrientation;
			//! La rotation sur les axes X et Y.
			Angles m_angles;
			//! La vitesse de rotation sur l'axe X.
			castor::RangedValue< castor::Angle > m_angularVelocityX
			{
				0.0_degrees,
				castor::makeRange( -5.0_degrees, 5.0_degrees )
			};
			//! La vitesse de rotation sur l'axe Y.
			castor::RangedValue< castor::Angle > m_angularVelocityY
			{
				0.0_degrees,
				castor::makeRange( -5.0_degrees, 5.0_degrees )
			};
			//! La vitesse de translation sur l'axe X.
			castor::RangedValue< float > m_scalarVelocityX
			{
				0.0f,
				castor::makeRange( -5.0f, 5.0f )
			};
			//! La vitesse de translation sur l'axe Y.
			castor::RangedValue< float > m_scalarVelocityY
			{
				0.0f,
				castor::makeRange( -5.0f, 5.0f )
			};
			//! La vitesse de translation sur l'axe Z.
			castor::RangedValue< float > m_scalarVelocityZ
			{
				0.0f,
				castor::makeRange( -5.0f, 5.0f )
			};
		};
		using NodeStatePtr = std::unique_ptr< NodeState >;
	}
}

#endif
