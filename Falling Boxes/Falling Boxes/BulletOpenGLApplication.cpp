#include "stdafx.h"
#include "BulletOpenGLApplication.h"
#include <iostream>

#define CAMERA_STEP_SIZE 5.0f

BulletOpenGLApplication::BulletOpenGLApplication()
{
	std::cout << "Constructing BulletOpenGLApplication and building camera" << std::endl;
	// Create Camera manager
	m_cameraManager = new CameraManager(
		btVector3(10.0f, 5.0f, 0.0f),	// Position
		btVector3(0.0f, 0.0f, 0.0f),	// Target
		15.0f,							// Distance
		20.0f,							// Pitch
		0.0f,							// Yaw
		btVector3(0.0f, 1.0f, 0.0f),	// Up Vector
		1.0f,							// near plane
		1000.0f);						// far plane

}

BulletOpenGLApplication::~BulletOpenGLApplication() {
	// Shutdown physics system
	ShutdownPhysics();
}

void BulletOpenGLApplication::Initialize() {
	std::cout << "Initializing BulletOpenGLApplication" << std::endl;
	// this function is called inside glutmain() after
	// creating the window, but before handing control
	// to FreeGLUT

	// create some floats for our ambient, diffuse, specular and position
	GLfloat ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // dark grey
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // white
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // white
	GLfloat position[] = { 5.0f, 10.0f, 1.0f, 0.0f };

	// set the ambient, diffuse, specular and position for LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING); // enables lighting
	glEnable(GL_LIGHT0); // enables the 0th light
	glEnable(GL_COLOR_MATERIAL); // colors materials when lighting is enabled

	// enable specular lighting via materials
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMateriali(GL_FRONT, GL_SHININESS, 15);

	// enable smooth shading
	glShadeModel(GL_SMOOTH);

	// enable depth testing to be 'less than'
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// set the backbuffer clearing color to a lightish blue
	glClearColor(0.6, 0.65, 0.85, 0);

	// initialize the physics system
	InitializePhysics();
}

void BulletOpenGLApplication::Keyboard(unsigned char key, int x, int y) {
	// This function is called by FreeGLUT whenever
	// generic keys are pressed down.
	switch (key) {
		// 'z' zooms in
	case 'z': m_cameraManager->ZoomCamera(+CAMERA_STEP_SIZE); break;
		// 'x' zoom out
	case 'x': m_cameraManager->ZoomCamera(-CAMERA_STEP_SIZE); break;
	}
}

void BulletOpenGLApplication::KeyboardUp(unsigned char key, int x, int y) {}

void BulletOpenGLApplication::Special(int key, int x, int y) {
	// This function is called by FreeGLUT whenever special keys
	// are pressed down, like the arrow keys, or Insert, Delete etc.
	switch (key) {
		// the arrow keys rotate the camera up/down/left/right
	case GLUT_KEY_LEFT:
		m_cameraManager->RotateCamera(YAW, +CAMERA_STEP_SIZE); break;
	case GLUT_KEY_RIGHT:
		m_cameraManager->RotateCamera(YAW, -CAMERA_STEP_SIZE); break;
	case GLUT_KEY_UP:
		m_cameraManager->RotateCamera(ROLL, +CAMERA_STEP_SIZE); break;
	case GLUT_KEY_DOWN:
		m_cameraManager->RotateCamera(ROLL, -CAMERA_STEP_SIZE); break;
	}
}

void BulletOpenGLApplication::SpecialUp(int key, int x, int y) {}

void BulletOpenGLApplication::Reshape(int w, int h) {
	// this function is called once during application intialization
	// and again every time we resize the window

	// grab the screen width/height
	m_cameraManager->setScreenWidth(w);
	m_cameraManager->setScreenHeight(h);
	// set the viewport
	glViewport(0, 0, w, h);
	// update the camera
	m_cameraManager->UpdateCamera();
}

void BulletOpenGLApplication::Idle() {
	// this function is called frequently, whenever FreeGlut
	// isn't busy processing its own events. It should be used
	// to perform any updating and rendering tasks

	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the time since the last iteration
	float dt = m_clock.getTimeMilliseconds();
	// reset the clock to 0
	m_clock.reset();
	// update the scene (convert ms to s)
	UpdateScene(dt / 1000.0f);

	// update the camera
	m_cameraManager->UpdateCamera();

	// render the scene
	RenderScene();

	// swap the front and back buffers
	glutSwapBuffers();
}

void BulletOpenGLApplication::Mouse(int button, int state, int x, int y) {}
void BulletOpenGLApplication::PassiveMotion(int x, int y) {}
void BulletOpenGLApplication::Motion(int x, int y) {}
void BulletOpenGLApplication::Display() {}

void BulletOpenGLApplication::DrawBox(const btVector3 &halfSize) {
	float halfWidth = halfSize.x();
	float halfHeight = halfSize.y();
	float halfDepth = halfSize.z();

	// Create vertex positions
	btVector3 vertices[8] = {
		btVector3(halfWidth, halfHeight, halfDepth),
		btVector3(halfWidth, halfHeight, -halfDepth),
		btVector3(halfWidth, -halfHeight, halfDepth),
		btVector3(halfWidth, -halfHeight, -halfDepth),
		btVector3(-halfWidth, halfHeight, halfDepth),
		btVector3(-halfWidth, halfHeight, -halfDepth),
		btVector3(-halfWidth, -halfHeight, halfDepth),
		btVector3(-halfWidth, -halfHeight, -halfDepth)
	};

	// create the indexes for each triangle, using the 
	// vertices above. Make it static so we don't waste 
	// processing time recreating it over and over again
	static int indices[36] = {
			0, 1, 2,
			3, 2, 1,
			4, 0, 6,
			6, 0, 2,
			5, 1, 4,
			4, 1, 0,
			7, 3, 1,
			7, 1, 5,
			5, 4, 7,
			7, 4, 6,
			7, 2, 3,
			7, 6, 2 };

	// start processing vertices as triangles
	glBegin(GL_TRIANGLES);

	// increment the loop by 3 each time since we create a 
	// triangle with 3 vertices at a time.

	for (int i = 0; i < 36; i += 3) {
		// get the three vertices for the triangle based
		// on the index values set above
		// use const references so we don't copy the object
		// (a good rule of thumb is to never allocate/deallocate
		// memory during *every* render/update call. This should 
		// only happen sporadically)
		const btVector3 &vert1 = vertices[indices[i]];
		const btVector3 &vert2 = vertices[indices[i + 1]];
		const btVector3 &vert3 = vertices[indices[i + 2]];

		// create a normal that is perpendicular to the 
		// face (use the cross product)
		btVector3 normal = (vert3 - vert1).cross(vert2 - vert1);
		normal.normalize();

		// set the normal for the subsequent vertices
		glNormal3f(normal.getX(), normal.getY(), normal.getZ());

		// create the vertices
		glVertex3f(vert1.x(), vert1.y(), vert1.z());
		glVertex3f(vert2.x(), vert2.y(), vert2.z());
		glVertex3f(vert3.x(), vert3.y(), vert3.z());
	}

	// stop processing vertices
	glEnd();
}

void BulletOpenGLApplication::RenderScene() {

	// create an array of 16 floats (representing a 4x4 matrix)
	btScalar transform[16];

	// iterate through all of the objects in our world
	for (GameObjects::iterator i = m_objects.begin(); i != m_objects.end(); ++i) {
		// get the object from the iterator
		GameObject* pObj = *i;

		// read the transform
		pObj->GetTransform(transform);

		// get data from the object and draw it
		DrawShape(transform, pObj->GetShape(), pObj->GetColor());
	}
}

void BulletOpenGLApplication::UpdateScene(float dt) {
	// check if the world object exists
	if (m_pWorld) {
		// step the simulation through time. This is called
		// every update and the amount of elasped time was 
		// determined back in ::Idle() by our clock object.
		m_pWorld->stepSimulation(dt);
	}
}

void BulletOpenGLApplication::DrawShape(btScalar *transform, const btCollisionShape *pShape, const btVector3 &color) {

	glColor3f(color.x(), color.y(), color.z());

	// push the matrix stack

	glPushMatrix();
	glMultMatrixf(transform);

	// make a different draw call based on object type
	switch (pShape->getShapeType())
	{
	case BOX_SHAPE_PROXYTYPE: {
		// assume the shape is a box, and typecast it
		const btBoxShape *box = static_cast<const btBoxShape*>(pShape);
		// get halfSize of the box
		btVector3 halfSize = box->getHalfExtentsWithMargin();
		// draw the box
		DrawBox(halfSize);
	}
	default:
		// unsupported type
		break;
	}

	glPopMatrix();

}

GameObject* BulletOpenGLApplication::CreateGameObject(
	btCollisionShape *pShape, 
	const float &mass, 
	const btVector3 &color, 
	const btVector3 &initialPosition, 
	const btQuaternion &initialRotation) {

	GameObject* pObject = new GameObject(pShape, mass, color, initialPosition, initialRotation);
	// push it to the back of the list
	m_objects.push_back(pObject);

	// check if the world object is valid
	if (m_pWorld) {
		// add the object's rigid body to the world
		m_pWorld->addRigidBody(pObject->GetRigidBody());

	}

	return pObject;

}