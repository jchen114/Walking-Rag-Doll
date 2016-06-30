#include "stdafx.h"
#include "WalkingActionInterface.h"
#include "WalkingController.h"

WalkingActionInterface::WalkingActionInterface(WalkingController *controller)
{
	m_controller = controller;
}


WalkingActionInterface::~WalkingActionInterface()
{
}

void WalkingActionInterface::updateAction(btCollisionWorld *collisionWorld, btScalar deltaTimeStep) {
	m_controller->StateLoop();
}