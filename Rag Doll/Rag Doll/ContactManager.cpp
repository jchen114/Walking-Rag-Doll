#include "stdafx.h"
#include "ContactManager.h"


#pragma region INITIALIZE

ContactManager::ContactManager()
{
}

ContactManager::~ContactManager()
{
}

btOverlapFilterCallback *ContactManager::GetFilterCallback() {
	static bool initialized;
	if (!initialized) {
		m_filterCallback = new ContactFilterCallback();
		initialized = true;
	}
	return m_filterCallback;
}

#pragma endregion INITIALIZE

#pragma region CONTACT_INTERFACE

void ContactManager::AddObjectForCollision(GameObject *object) {
	
	m_beingUsed = true;

	ColliderObject colliderObject(object);
	m_forCollision.insert({ object, colliderObject });
}

void ContactManager::AddObjectToCollideWith(GameObject *object){
	CollideeObject collideeObject(object);
	m_toCollideWith.insert({ object, collideeObject });
}

int ContactManager::RemoveObjectForCollision(GameObject *object) {
	// TODO
	return 0;
}

int ContactManager::RemoveObjectToCollideWith(GameObject *object) {
	// TODO
	return 0;
}

#pragma endregion CONTACT_INTERFACE

#pragma region DRAWING

void ContactManager::DrawContactPoints() {
	for (auto it = m_forCollision.begin(); it != m_forCollision.end(); it++) {
		ColliderObject colliderObject = it->second;
		colliderObject.DrawAndLabelContactPoints();
	}
}

#pragma endregion DRAWING

void ContactManager::Update(btScalar timestep) {

	std::vector<CollideeObject> objs;
	objs.reserve(m_toCollideWith.size());

	for (auto kv : m_toCollideWith) {
		CollideeObject obj = kv.second;
		obj.UpdateCollidingPlane();
		objs.push_back(obj);
	}

	for (auto it = m_forCollision.begin(); it != m_forCollision.end(); it++) {
		ColliderObject colliderObject = it->second;
		colliderObject.CollisionDetectionUpdate(objs);
	}

}

void ContactManager::AddCollisionPair(GameObject *obj1, GameObject *obj2) {

	m_collisionPairs.insert({ obj1, obj2 });

}

void ContactManager::RemoveCollisionPair(GameObject *obj) {

	m_collisionPairs.erase(obj);

}