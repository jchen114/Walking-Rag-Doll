#include "stdafx.h"
#include "ColliderObject.h"

#include "ColliderVertex.h"
#include "CollideeObject.h"


#pragma region INITIALIZE

ColliderObject::ColliderObject(GameObject *object)
{
	m_object = object;
	m_userPointer = object;

	m_object->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

	switch (m_object->GetRigidBody()->getCollisionShape()->getShapeType()) {
	case BOX_2D_SHAPE_PROXYTYPE:
	{
		Initialize2DBox();
	}
		break;
	default:
		break;
	}
}


ColliderObject::~ColliderObject()
{
}

void ColliderObject::Initialize2DBox() {

	const btBox2dShape *box = static_cast<const btBox2dShape*>(m_object->GetRigidBody()->getCollisionShape());
	btVector3 halfSize = box->getHalfExtentsWithMargin();

	// Get vertices in a clockwise manner.
	m_vertices = GetVertexPositionsFor2DBox(halfSize);

}


void ColliderObject::CollisionDetectionUpdate(std::vector<CollideeObject> collidees) {

	for (auto it = m_vertices.begin(); it != m_vertices.end(); it++) {
		ColliderVertex* vertex = *it;
		vertex->CollisionDetectionUpdate(collidees);
	}

}

#pragma endregion INITIALIZE

void ColliderObject::DrawAndLabelContactPoints() {

	// Draw the forces present on the vertices
	for (auto it = m_vertices.begin(); it != m_vertices.end(); it++) {
		ColliderVertex *vertex = *it;
		vertex->DrawInfo();
		vertex->DrawForce();
	}

}

std::vector<ColliderVertex*> ColliderObject::GetVertexPositionsFor2DBox( const btVector3 &halfSize) {

	std::vector<ColliderVertex*> vertex_positions = {
		new ColliderVertex(m_object, btVector3(-halfSize.x(), -halfSize.y(), 0), 0),
		new ColliderVertex(m_object, btVector3(-halfSize.x(), halfSize.y(), 0), 1),
		new ColliderVertex(m_object, btVector3(halfSize.x(), halfSize.y(), 0), 2),
		new ColliderVertex(m_object, btVector3(halfSize.x(), -halfSize.y(), 0), 3)
	};
	return vertex_positions;
}

std::vector<btVector3> ColliderObject::GetVertexVelocitiesFor2DBox(btRigidBody *body, const btVector3 &halfSize) {
	std::vector<btVector3> vertex_velocities = {
		body->getVelocityInLocalPoint(btVector3(-halfSize.x(), -halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(-halfSize.x(), halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(halfSize.x(), halfSize.y(), 0)),
		body->getVelocityInLocalPoint(btVector3(halfSize.x(), -halfSize.y(), 0))
	};
	return vertex_velocities;
}