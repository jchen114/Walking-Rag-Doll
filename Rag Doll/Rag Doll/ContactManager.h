#pragma once

#include "GameObject.h"
#include <unordered_set>
#include "BulletCollision\CollisionShapes\btBox2dShape.h"
#include "BulletOpenGLApplication.h"

#include "ColliderObject.h"
#include "CollideeObject.h"

#include <unordered_map>

enum CollisionTypes {
	COL_CONTACT_MODEL = 128
};

class ContactManager
{

public:

	static ContactManager& GetInstance() {
		static ContactManager instance;
		return instance;
	}

	ContactManager();
	~ContactManager();

	void AddObjectForCollision(GameObject *object);
	void AddObjectToCollideWith(GameObject *object);

	int RemoveObjectForCollision(GameObject *object);
	int RemoveObjectToCollideWith(GameObject *object);

	btOverlapFilterCallback *GetFilterCallback();

	void DrawContactPoints();

	std::unordered_map<GameObject *, ColliderObject> m_forCollision;
	std::unordered_map<GameObject *, CollideeObject> m_toCollideWith;

	void Update(btScalar timestep);

	void AddCollisionPair(GameObject *obj1, GameObject *obj2);
	void RemoveCollisionPair(GameObject *obj);

	std::unordered_map<GameObject *, GameObject *> m_collisionPairs;

	bool m_beingUsed = false;

private:

	btOverlapFilterCallback *m_filterCallback;
	

};

struct ContactFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		btCollisionObject *obj0 = (btCollisionObject *)proxy0->m_clientObject;
		btCollisionObject *obj1 = (btCollisionObject *)proxy1->m_clientObject;
		if (ContactManager::GetInstance().m_forCollision.find((GameObject *)obj0->getUserPointer()) != ContactManager::GetInstance().m_forCollision.end()
			&& ContactManager::GetInstance().m_toCollideWith.find((GameObject *)obj1->getUserPointer()) != ContactManager::GetInstance().m_toCollideWith.end()) {
			//printf("object 0 for collision object 1 to collide\n");
			return false;
			//return true;
		}
		else if (
			ContactManager::GetInstance().m_forCollision.find((GameObject *)obj1->getUserPointer()) != ContactManager::GetInstance().m_forCollision.end()
			&& ContactManager::GetInstance().m_toCollideWith.find((GameObject *)obj0->getUserPointer()) != ContactManager::GetInstance().m_toCollideWith.end()) {
			//printf("object 0 to collide object 1 for collision \n");
			return false;
			//return true;
		}
		return true;

	}
};
