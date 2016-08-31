#pragma once

#include "GameObject.h"
#include "BulletCollision\CollisionShapes\btBox2dShape.h"

typedef enum {
	COLLIDEE_BOX_2D_SHAPE,
	COLLIDEE_BOX_SHAPE
} ColliderShapeType;

class CollideeObject
{
public:
	CollideeObject(GameObject *object);
	~CollideeObject();

	void *m_userPointer;
	ColliderShapeType m_shapeType;
	GameObject *m_object;

	void UpdateCollidingPlane();
	std::vector <std::pair<btVector3, btVector3>> GetPlanes();
	

private:

	std::vector <std::pair<btVector3, btVector3>> HandleBoxCollidingPlanes();
	std::vector <std::pair<btVector3, btVector3>> Handle2DBoxCollidingPlanes();
	std::vector <std::pair<btVector3, btVector3>> m_planes;

};

