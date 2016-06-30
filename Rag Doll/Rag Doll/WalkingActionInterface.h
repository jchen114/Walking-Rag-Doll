#pragma once
#include "BulletDynamics\Dynamics\btActionInterface.h"

class WalkingController;
class State;
class Gains;

class WalkingActionInterface :
	public btActionInterface
{
public:
	WalkingActionInterface(WalkingController *controller);
	~WalkingActionInterface();

	virtual void updateAction(btCollisionWorld *collisionWorld, btScalar deltaTimeStep) override;
	virtual void debugDraw(btIDebugDraw *debugDrawer) {

	}

private:
	WalkingController *m_controller;

};

