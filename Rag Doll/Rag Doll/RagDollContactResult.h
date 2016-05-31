#pragma once
#include "BulletCollision\CollisionDispatch\btCollisionWorld.h"

class WalkingController;

enum ContactType{LEFT_FOOT_GROUND = 200, RIGHT_FOOT_GROUND};

class RagDollContactResult :
	public btCollisionWorld::ContactResultCallback
{
public:
	RagDollContactResult(WalkingController *controller, ContactType contactType);
	~RagDollContactResult();

	virtual btScalar addSingleResult(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1) override;

	WalkingController *m_walkingController;

	ContactType m_contactType;

};

