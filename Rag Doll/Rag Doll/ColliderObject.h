#pragma once

#include "GameObject.h"

#include "BulletCollision\CollisionShapes\btBox2dShape.h"
#include "BulletOpenGLApplication.h"

typedef enum
{
	COLLIDER_BOX_2D_SHAPE = 0,
	COLLIDER_BOX_SHAPE
} ColliderType;

class ColliderVertex;
class CollideeObject;

class ColliderObject
{
public:
	ColliderObject(GameObject *object);
	~ColliderObject();

	ColliderType m_colliderType;
	GameObject *m_object;

	std::vector<ColliderVertex *> m_vertices;

	void DrawAndLabelContactPoints();

	void *m_userPointer;

	void CollisionDetectionUpdate(std::vector<CollideeObject> collidees);

	std::vector<ColliderVertex *> GetVertexPositionsFor2DBox(const btVector3 &halfSize);
	std::vector<btVector3> GetVertexVelocitiesFor2DBox(btRigidBody *body, const btVector3 &halfSize);

private:
	void Initialize2DBox();
	
};

