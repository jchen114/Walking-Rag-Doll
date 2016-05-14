#ifndef _BULLETOPENGLAPP_H_
#define _BULLETOPENGLAPP_H_

#include <Windows.h>
#include <gl\GL.h>
#include <freeglut\freeglut.h>

#include "BulletDynamics\Dynamics\btDynamicsWorld.h"

// Includes for 2D Boxes and collision between 2D Boxes
#include "BulletCollision\CollisionShapes\btBox2dShape.h"
#include "BulletCollision\CollisionDispatch\btBox2dBox2dCollisionAlgorithm.h"

// Our custom debug renderer
#include "DebugDrawer.h"

// Includes a custom motion state object
#include "OpenGLMotionState.h"

#include "CameraManager.h"

#include "GameObject.h"
#include <vector>

#define CAMERA_STEP_SIZE 0.3f

typedef std::vector<GameObject*> GameObjects; // GameObjects is a data type for storing game objects

class BulletOpenGLApplication
{
public:
	BulletOpenGLApplication();
	BulletOpenGLApplication(ProjectionType);

	~BulletOpenGLApplication();

	void Initialize();

	// FreeGLUT callbacks //
	virtual void Keyboard(unsigned char key, int x, int y);
	virtual void KeyboardUp(unsigned char key, int x, int y);
	virtual void Special(int key, int x, int y);
	virtual void SpecialUp(int key, int x, int y);
	virtual void Reshape(int w, int h);
	virtual void Idle();
	virtual void Mouse(int button, int state, int x, int y);
	virtual void PassiveMotion(int x, int y);
	virtual void Motion(int x, int y);
	virtual void Display();

	// rendering. Can be overrideen by derived classes
	virtual void RenderScene();

	// scene updating. Can be overridden by derived classes
	virtual void UpdateScene(float dt);

	// physics functions. Can be overridden by derived classes (like BasicDemo)
	virtual void InitializePhysics() {};
	virtual void ShutdownPhysics() {};

	// Drawing Functions
	void DrawBox(const btVector3 &halfSize);
	void DrawPlane(const btVector3 &halfSize);
	void DrawShape(btScalar *transform, const btCollisionShape *pShape, const btVector3 &color);
	void DrawWithTriangles(const btVector3 * vertices, const int *indices, int numberOfIndices);

	void SetScreenWidth(int screenWidth);
	void SetScreenHeight(int screenHeight);

	// Object Functions
	GameObject *CreateGameObject(
		btCollisionShape *pShape, 
		const float &mass, 
		const btVector3 &color = btVector3(1.0f, 1.0f, 1.0f), 
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 1, 1)
		);

protected:

	// core Bullet Components
	btBroadphaseInterface *m_pBroadphase;
	btCollisionConfiguration *m_pCollisionConfiguration;
	btCollisionDispatcher *m_pDispatcher;
	btConstraintSolver *m_pSolver;
	btDynamicsWorld *m_pWorld;

	// clock for counting time
	btClock m_clock;

	// Array for game objects
	GameObjects m_objects;

	// Camera Manager
	CameraManager *m_cameraManager;

	// Debugging
	// debug renderer
	DebugDrawer* m_pDebugDrawer;

	float m_screenWidth;
	float m_screenHeight;

	float GetPixelsToMeters(float distanceToCamera);

};

#endif
