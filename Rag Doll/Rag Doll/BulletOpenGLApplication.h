#ifndef _BULLETOPENGLAPP_H_
#define _BULLETOPENGLAPP_H_

#include <Windows.h>
#include <gl\GL.h>
#include <freeglut\freeglut.h>
#include <functional>

#include "BulletDynamics\Dynamics\btDynamicsWorld.h"

// Includes for 2D Boxes and collision between 2D Boxes
#include "BulletCollision\CollisionShapes\btBox2dShape.h"
#include "BulletCollision\CollisionDispatch\btBox2dBox2dCollisionAlgorithm.h"

// Our custom debug renderer
#include "DebugDrawer.h"

// Includes a custom motion state object
#include "OpenGLMotionState.h"

#include "CameraManager.h"

// Constants
#include "Constants.h"

#include "GameObject.h"
#include <vector>

#define RENDER_TIME_STEP 0.02f // 20 ms = 50fps

static float desiredTimeBetweenFrames = 1.0f / 60.0f;

//Debug
//#define USEDEBUG

#ifdef USEDEBUG
//#define Debug( x ) std::cout << x << std::endl
#else
#define Debug( x ) 
#endif

// Realtime
#define REALTIME

#ifdef REALTIME
#define BULLET_TIME_STEP 0.0005f // 2khz
#else
//#define BULLET_TIME_STEP 0.001f // 1 ms
#define NUM_STEPS 11
#endif

class DebugDrawer;

typedef std::vector<GameObject*> GameObjects; // GameObjects is a data type for storing game objects

class BulletOpenGLApplication
{
public:
	BulletOpenGLApplication();
	BulletOpenGLApplication(ProjectionMode mode, bool isFrameRateFixed = false);

	~BulletOpenGLApplication();

	void Initialize();

	int m_main_window_id = 0;

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
	virtual void GLUTTimerFunc(int value);
	virtual void DrawShape(btScalar *transform, const btCollisionShape *pShape, const btVector3 &color);

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
	
	void DrawWithTriangles(const btVector3 * vertices, const int *indices, int numberOfIndices);

	void SetScreenWidth(int width);
	void SetScreenHeight(int height);

	// Object Functions

	btHingeConstraint *AddHingeConstraint(
		GameObject *obj1, 
		GameObject *obj2, 
		const btVector3 &pivot1, 
		const btVector3 &pivot2, 
		const btVector3 &axis1, 
		const btVector3 &axis2,
		btScalar lowLimit,
		btScalar highLimit);

	btFixedConstraint *AddFixedConstraint(
		GameObject *obj1,
		GameObject *obj2,
		const btTransform &trans1 = btTransform(btQuaternion(0, 0, 0, 1)),
		const btTransform &trans2 = btTransform(btQuaternion(0, 0, 0, 1))
		);

	void ApplyTorque(GameObject *object, const btVector3 &torque);

	GameObject *CreateGameObject(
		btCollisionShape *pShape, 
		const float &mass, 
		const btVector3 &color = btVector3(1.0f, 1.0f, 1.0f), 
		const btVector3 &initialPosition = btVector3(0.0f, 0.0f, 0.0f),
		const btQuaternion &initialRotation = btQuaternion(0, 0, 0, 1)
		);	

	// Callback for drawing
	std::function<void()> m_DrawCallback;
	std::function<void(btScalar *, const btCollisionShape *, const btVector3 &)> m_DrawShapeCallback;

	float m_DeltaGlutTime;
	float m_DeltaSimTime;

protected:

	// core Bullet Components
	btBroadphaseInterface *m_pBroadphase;
	btCollisionConfiguration *m_pCollisionConfiguration;
	btCollisionDispatcher *m_pDispatcher;
	btConstraintSolver *m_pSolver;
	btDynamicsWorld *m_pWorld;

	// clock for counting time
	btClock m_clock;
	// clock for timing simulation
	btClock m_SimClock;
	// Array for game objects
	GameObjects m_objects;

	// Camera Manager
	CameraManager *m_cameraManager;

	// Debugging
	// debug renderer
	DebugDrawer* m_pDebugDrawer;

	bool m_IsFrameRateFixed;
	float m_RemainingTime = 0.0f;

	int m_previous_number_of_steps = 0;

};

static BulletOpenGLApplication *m_me;

static void GLUTTimerCallback(int value) {
	m_me->GLUTTimerFunc(value);
}

#endif
