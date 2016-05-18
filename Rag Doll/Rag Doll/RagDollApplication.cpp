#include "stdafx.h"
#include "RagDollApplication.h"


RagDollApplication::RagDollApplication()
{
}

RagDollApplication::RagDollApplication(ProjectionMode mode) :BulletOpenGLApplication(mode){}


RagDollApplication::~RagDollApplication()
{
}

void RagDollApplication::InitializePhysics() {

	// create the collision configuration
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	// Adding for 2D collisions and solving
	m_pDispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, new btBox2dBox2dCollisionAlgorithm::CreateFunc());

	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	// Create ground
	btVector3 ground(1.0f, 25.0f, 10.0f);
	float mass = 0.0f;
	btVector3 position(0.0f, -10.0f, 0.0f);
	Create3DBox(ground, mass, GetRandomColor(), position);

	CreateRagDoll(btVector3(0, 3, 0.5));

}

void RagDollApplication::CreateRagDoll(const btVector3 &position) {

	// MASS
	float torso_mass = 70;
	float upper_leg_mass = 5;
	float lower_leg_mass = 4;
	float feet_mass = 1;

	// DIMENSIONS
	float torso_width = 1;
	float torso_height = 2.5;

	float upper_leg_height = 3.0f;
	float upper_leg_width = 0.7f;

	float lower_leg_height = 3.0f;
	float lower_leg_width = 0.5f;

	float foot_height = 0.3;
	float foot_width = 0.9;

	// Create a torso centered at the position
	btVector3 halfSize(torso_height/2, torso_width/2, 0.0);
	
	GameObject *torso = Create2DBox(halfSize, torso_mass, GetRandomColor(), position);

	// test box
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(-4, 5, 0.7));
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(4, 7, 0.5));

	// Create Upper legs
	halfSize = btVector3(upper_leg_height / 2, upper_leg_width / 2, 0.0f);
	btVector3 pos1 = position;
	pos1.setZ(pos1.getZ() - 0.25);
	GameObject *leftUpperLeg = Create2DBox(halfSize, upper_leg_mass, GetRandomColor(), pos1);
	btVector3 pos2 = position;
	pos2.setZ(pos2.getZ() + 0.25);
	GameObject *rightUpperLeg = Create2DBox(halfSize, upper_leg_mass, GetRandomColor(), pos2);

	// Create lower legs
	halfSize = btVector3(lower_leg_height / 2, lower_leg_width / 2, 0.0f);
	btVector3 pos3 = position;
	pos3.setZ(pos3.getZ() - 0.27);
	GameObject *leftLowerLeg = Create2DBox(halfSize, lower_leg_mass, GetRandomColor(), pos3);
	btVector3 pos4 = position;
	pos4.setZ(pos4.getZ() + 0.27);
	GameObject *rightLowerLeg = Create2DBox(halfSize, lower_leg_mass, GetRandomColor(), pos4);

	// Create feet
	halfSize = btVector3(foot_height / 2, foot_width / 2, 0.0f);
	btVector3 pos5 = position;
	pos5.setZ(pos5.getZ() - 0.28);
	GameObject *leftFoot = Create2DBox(halfSize, feet_mass, GetRandomColor(), pos5);
	halfSize = btVector3(foot_height / 2, foot_width / 2, 0.0f);
	btVector3 pos6 = position;
	pos6.setZ(pos6.getZ() + 0.28);
	GameObject *rightFoot = Create2DBox(halfSize, feet_mass, GetRandomColor(), pos6);

	// Connect torso to upper legs
	AddHingeConstraint(torso, leftUpperLeg, btVector3(-1, 0, 0), btVector3(1, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(-30.0f), Constants::GetInstance().DegreesToRadians(-30.0f));
	AddHingeConstraint(torso, rightUpperLeg, btVector3(-1, 0, 0), btVector3(1, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(30.0f), Constants::GetInstance().DegreesToRadians(30.0f));

	// Connect upper legs to lower legs
	AddHingeConstraint(leftUpperLeg, leftLowerLeg, btVector3(-1.5, 0, 0), btVector3(1, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(0.0f));
	AddHingeConstraint(rightUpperLeg, rightLowerLeg, btVector3(-1.5, 0, 0), btVector3(1, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(0.0f));

	// Connect feet to lower legs
	AddHingeConstraint(leftLowerLeg, leftFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3(0, foot_width / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(20.0f), Constants::GetInstance().DegreesToRadians(20.0f));
	AddHingeConstraint(rightLowerLeg, rightFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3(0, foot_width / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(20.0f), Constants::GetInstance().DegreesToRadians(20.0f));
}


void RagDollApplication::ShutdownPhysics() {

}

GameObject *RagDollApplication::Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {

	GameObject *aBox = CreateGameObject(new btBox2dShape(halfSize), mass, color, position);
	return aBox;
}

GameObject *RagDollApplication::Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {
	GameObject *aBox = CreateGameObject(new btBoxShape(halfSize), mass, color, position);
	return aBox;
}


btVector3 RagDollApplication::GetRandomColor() {
	return btVector3(((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX));
}