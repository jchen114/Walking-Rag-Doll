#include "stdafx.h"
#include "RagDollContactResult.h"

#include "WalkingController.h"


RagDollContactResult::RagDollContactResult(WalkingController *controller, ContactType contactType)
{
	m_walkingController = controller;
	m_contactType = contactType;
}


RagDollContactResult::~RagDollContactResult()
{
}


btScalar RagDollContactResult::addSingleResult(
	btManifoldPoint &cp, 
	const btCollisionObjectWrapper *colObj0Wrap, 
	int partId0, 
	int index0,
	const btCollisionObjectWrapper *colObj1Wrap, 
	int partId1, 
	int index1) {

	printf("Collision occured. \n");

	switch (m_contactType)
	{
	case LEFT_FOOT_GROUND: {
		m_walkingController->m_leftFootContact = true;
	}
		break;
	case RIGHT_FOOT_GROUND: {
		m_walkingController->m_rightFootContact = true;
	}
		break;
	default:
		break;
	}

}