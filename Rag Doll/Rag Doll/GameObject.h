#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "btBulletDynamicsCommon.h"

#include "OpenGLMotionState.h"
#include <vector>

class GameObject
{
public:
	// Constructor + Destructor
	GameObject(btCollisionShape *pShape,
		float mass,
		const btVector3 &color,
		const btVector3 &initialPosition = btVector3(0, 0, 0),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1));
	~GameObject();

	static void ClearForces(std::vector<GameObject *> objects) {
		for (std::vector<GameObject *>::iterator it = objects.begin(); it != objects.end(); ++it) {
			(*it)->GetRigidBody()->clearForces();
		}
	}

	static void ClearVelocities(std::vector<GameObject *>objects) {
		btVector3 zeroVec(0, 0, 0);
		for (std::vector<GameObject *>::iterator it = objects.begin(); it != objects.end(); ++it) {
			(*it)->GetRigidBody()->setLinearVelocity(zeroVec);
			(*it)->GetRigidBody()->setAngularVelocity(zeroVec);
		}
	}

	static void DisableObjects(std::vector<GameObject *> objects) {
		for (std::vector<GameObject *>::iterator it = objects.begin(); it != objects.end(); ++it) {
			//(*it)->GetRigidBody()->clearForces();
			// Make object static
			(*it)->GetRigidBody()->setMassProps(0.0f, btVector3(0, 0, 0));
		}
	}

	static void EnableObjects(std::vector<GameObject *> objects) {
		printf("ENABLE THE BODIES\n");
		for (std::vector<GameObject *>::iterator it = objects.begin(); it != objects.end(); ++it) {
			//(*it)->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
			// Set mass to original mass
			(*it)->GetRigidBody()->setMassProps((*it)->GetMass(), (*it)->GetInertia());
			(*it)->GetRigidBody()->activate();
		}
	}

	void Reposition(const btVector3 &position, const btQuaternion &orientation = btQuaternion(0, 0, 1, 1));


	// accessors
	btCollisionShape *GetShape() { return m_pShape; }
	btRigidBody *GetRigidBody() { return m_pBody; }
	btMotionState *GetMotionState() { return m_pMotionState; }
	btScalar GetMass() { return m_mass; }
	btVector3 GetInertia() { return m_inertia; }
	const btVector3 GetCOMPosition() {
		return m_pBody->getCenterOfMassPosition();
	}
	void GetTransform(btScalar *transform) {
		if (m_pMotionState) {
			m_pMotionState->GetWorldTransform(transform);
		}
	}

	btVector3 GetColor() { return m_color; }

protected:
	btCollisionShape *m_pShape;
	btRigidBody *m_pBody;
	OpenGLMotionState *m_pMotionState;
	btVector3 m_color;

	btVector3 m_inertia;
	btScalar m_mass;


};
#endif


