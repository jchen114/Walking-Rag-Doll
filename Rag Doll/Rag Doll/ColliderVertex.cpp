#include "stdafx.h"
#include "ColliderVertex.h"
#include "CollideeObject.h"
#include "ContactManager.h"



ColliderVertex::ColliderVertex(GameObject *object, const btVector3 &offset, int vid, btScalar friction)
{
	m_object = object;
	m_offset = offset;
	m_distanceFromCOM = m_offset.length();

	m_vertexPos = object->GetCOMPosition() + m_offset;
	m_vertexVel = object->GetRigidBody()->getVelocityInLocalPoint(m_offset);

	m_friction = friction;
	m_id = vid;

	m_minAngle = -atan(m_friction);
	m_maxAngle = atan(m_friction);

}


ColliderVertex::~ColliderVertex()
{
}

void ColliderVertex::CollisionDetectionUpdate(std::vector<CollideeObject> objects) {

	if (!m_object) {
		return;
	}

	btVector3 COM = m_object->GetCOMPosition();

	m_orientation = Constants::GetInstance().DegreesToRadians(m_object->GetOrientation());

	m_newOffset = m_offset.rotate(btVector3(0, 0, 1), m_orientation);

	m_vertexPos = COM + m_newOffset;
	m_vertexVel = m_object->GetRigidBody()->getVelocityInLocalPoint(m_newOffset);

	for (auto it = objects.begin(); it != objects.end(); it++) {
		CollideeObject object = *it;
		switch (object.m_shapeType)
		{
		case COLLIDEE_BOX_SHAPE: {
			//  Only implementing TOP plane for now..
			auto planes = object.GetPlanes();
			HandleBoxCollision(planes, object);
		}
			break;
		case COLLIDEE_BOX_2D_SHAPE: {
			auto planes = object.GetPlanes();
			Handle2DBoxCollision(planes, object);
		}
			break;
		default:
			break;
		}

		// Get the bounds from the collision objects to collide with.


	}
}

void ColliderVertex::HandleBoxCollision(std::vector<std::pair<btVector3, btVector3>> planes, CollideeObject collidee) {
	// Check if vertex penetrates the planes.
	// Only top plane for now...
	std::pair < btVector3, btVector3 > top_plane = planes.front();

	btVector3 v1 = top_plane.first;
	btVector3 v2 = top_plane.second;
	// top plane...
	if (m_vertexPos.x() > v1.x() && m_vertexPos.x() < v2.x()
		&& m_vertexPos.y() < v1.y()
		&& m_vertexPos.z() > v1.z() && m_vertexPos.z() < v2.z())
	{
		if (m_state == NO_COLLISION) {
			//printf("%d, Collision happened!\n", m_id);
			m_state = IN_COLLISION;
			m_collisionPoint = m_previousPoint;
		}
		// Spring force in direction towards penetration point
		m_springForce = m_collisionPoint - m_vertexPos;
		m_springForce = m_springForce.normalize();
		m_springForce = m_springForce * m_springConstant;

		m_dampingForce = m_vertexVel * m_dampingConstant;

		// Check reaction force for negative.
		m_reactionForce = m_springForce - m_dampingForce;
		m_reactionForce.y() < 0 ? m_reactionForce *= btVector3(0, 0, 0) : m_reactionForce;

		// Check to see if reaction force is inside cone of friction.
		float angle = atan(m_reactionForce.x() / m_reactionForce.y());
		if (angle < m_minAngle) { // negative angle
			m_reactionForce.setX(m_reactionForce.y() * tan(m_minAngle));
			float delta_x = (m_collisionPoint.y() - m_vertexPos.y()) * tan(m_minAngle);
			m_collisionPoint.setX(m_vertexPos.x() + delta_x);
		}
		if (angle > m_maxAngle) {
			m_reactionForce.setX(m_reactionForce.y() * tan(m_maxAngle));
			float delta_x = (m_collisionPoint.y() - m_vertexPos.y()) * tan(m_maxAngle);
			m_collisionPoint.setX(m_vertexPos.x() + delta_x);
		}

		NotifyCollision(m_object, collidee.m_object);

		m_object->GetRigidBody()->applyForce(m_reactionForce, m_newOffset);
	} else {

		if (m_state == IN_COLLISION) {
			NotifyExitCollision(m_object);
		}

		m_state = NO_COLLISION;
	}

	m_previousPoint = m_vertexPos;

}

void ColliderVertex::Handle2DBoxCollision(std::vector<std::pair<btVector3, btVector3>> planes, CollideeObject collidee) {
	std::pair < btVector3, btVector3 > top_plane = planes.front();
	btVector3 v1 = top_plane.first;
	btVector3 v2 = top_plane.second;

	// top plane... Assume horizontal
	if (m_vertexPos.x() > v1.x() && m_vertexPos.x() < v2.x()
		&& m_vertexPos.y() < v1.y())
	{
		if (m_state == NO_COLLISION) {
			//printf("%d, Collision happened!\n", m_id);
			m_state = IN_COLLISION;
			m_collisionPoint = m_previousPoint;
		}
		// Spring force in direction towards penetration point
		m_springForce = m_collisionPoint - m_vertexPos;
		m_springForce = m_springForce.normalize();
		m_springForce = m_springForce * m_springConstant;

		m_dampingForce = m_vertexVel * m_dampingConstant;

		// Check reaction force for negative.
		m_reactionForce = m_springForce - m_dampingForce;
		m_reactionForce.y() < 0 ? m_reactionForce *= btVector3(0, 0, 0) : m_reactionForce;

		// Check to see if reaction force is inside cone of friction.
		float angle = atan(m_reactionForce.x() / m_reactionForce.y());
		if (angle < m_minAngle) { // negative angle
			m_reactionForce.setX(m_reactionForce.y() * tan(m_minAngle));
			float delta_x = (m_collisionPoint.y() - m_vertexPos.y()) * tan(m_minAngle);
			m_collisionPoint.setX(m_vertexPos.x() + delta_x);
		}
		if (angle > m_maxAngle) {
			m_reactionForce.setX(m_reactionForce.y() * tan(m_maxAngle));
			float delta_x = (m_collisionPoint.y() - m_vertexPos.y()) * tan(m_maxAngle);
			m_collisionPoint.setX(m_vertexPos.x() + delta_x);
		}

		NotifyCollision(m_object, collidee.m_object);

		m_object->GetRigidBody()->applyForce(m_reactionForce, m_newOffset);
	}
	else {

		if (m_state == IN_COLLISION) {
			NotifyExitCollision(m_object);
		}

		m_state = NO_COLLISION;
	}

	m_previousPoint = m_vertexPos;

}

void ColliderVertex::DrawInfo() {
	btScalar transform[16];

	m_object->GetTransform(transform);

	glPushMatrix();
	glMultMatrixf(transform);

	glTranslatef(m_offset.x(), m_offset.y(), 0.1f);

	glColor3f(1.0f, 0.0f, 0.0f);
	DrawCircle(0.01f);

	//char buf[200];
	////sprintf_s(buf, "id: %d, P: (%3.3f, %3.3f, %3.3f), V:(%3.3f, %3.3f, %3.3f)", id, m_vertexPos.x(), m_vertexPos.y(), m_vertexPos.z(), m_vertexVel.x(), m_vertexVel.y(), m_vertexVel.z());
	//sprintf_s(buf, "id: %d, Col: %d, x: %3.3f, y: %3.3f, o: %f)", m_id, m_state == IN_COLLISION ? 1 : 0, m_vertexPos.x(), m_vertexPos.y(), Constants::GetInstance().RadiansToDegrees(m_orientation));
	//
	//DisplayText(0.0f, m_id *0.2 + 0.1, btVector3(0.0f, 0.0f, 0.0f), buf);

	glPopMatrix();
}

void ColliderVertex::DrawForce() {

	if (m_state == IN_COLLISION) {
		
		btVector3 COM = m_object->GetCOMPosition();

		glPushMatrix();
		glTranslatef(COM.x(), COM.y(), COM.z());

		glColor3f(0.0f, 0.0f, 1.0f);
		glLineWidth(3.0f);
		glBegin(GL_LINES);
		glVertex3f(m_newOffset.x(), m_newOffset.y(), 0.1);
		glVertex3f(m_newOffset.x() + m_reactionForce.x(), m_newOffset.y() + m_reactionForce.y(), 0.0f);
		glEnd();
		glPopMatrix();
	}

}

void ColliderVertex::NotifyCollision(GameObject *obj1, GameObject *obj2) {
	ContactManager::GetInstance().AddCollisionPair(obj1, obj2);
}

void ColliderVertex::NotifyExitCollision(GameObject *obj) {
	ContactManager::GetInstance().RemoveCollisionPair(obj);
}
