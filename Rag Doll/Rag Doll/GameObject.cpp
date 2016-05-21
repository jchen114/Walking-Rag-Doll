#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject(	btCollisionShape *pShape,
						float mass,
						const btVector3 &color,
						const btVector3 &initialPosition,
						const btQuaternion &initialRotation) {

	m_pShape = pShape;	// Store the shape 
	m_color = color;	// Store the color

	// Create initial transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(initialPosition);
	transform.setRotation(initialRotation);

	// Create Motion State from the initial Transform
	m_pMotionState = new OpenGLMotionState(transform);
	
	// Calculate the local inertia
	btVector3 localInertia(0, 0, 0);

	// Objects of infinite mass can't move or rotate
	if (mass != 0.0f) {
		pShape->calculateLocalInertia(mass, localInertia);
	}

	// create the rigid body construction info using mass, motion state, and shape
	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, m_pMotionState, pShape, localInertia);

	// create the rigid body
	m_pBody = new btRigidBody(cInfo);
}


void GameObject::Reposition(const btVector3 &position, const btQuaternion &orientation) {

	btTransform initialTransform;
	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);

	m_pBody->setWorldTransform(initialTransform);
	m_pMotionState->setWorldTransform(initialTransform);

}

GameObject::~GameObject()
{
	delete m_pBody;
	delete m_pMotionState;
	delete m_pShape;
}
