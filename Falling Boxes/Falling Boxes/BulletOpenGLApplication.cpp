#include "stdafx.h"
#include "BulletOpenGLApplication.h"
#include <iostream>

BulletOpenGLApplication::BulletOpenGLApplication()
{
	std::cout << "Constructing BulletOpenGLApplication and building camera" << std::endl;
	// Create Camera manager
	m_cameraManager = new CameraManager(
		btVector3(0.0f, 0.0f, 0.0f),	// Target
		30.0f,							// Distance
		20.0f,							// Pitch
		0.0f,							// Yaw
		btVector3(0.0f, 1.0f, 0.0f),	// Up Vector
		1.0f,							// near plane
		1000.0f);						// far plane
}

BulletOpenGLApplication::BulletOpenGLApplication(ProjectionType mode) {
	std::cout << "Constructing BulletOpenGLApplication and building camera" << std::endl;
	// Create Camera manager
	m_cameraManager = new CameraManager(
		btVector3(0.0f, 0.0f, 0.0f),	// Target
		30.0f,							// Distance
		0.0f,							// Pitch
		0.0f,							// Yaw
		btVector3(0.0f, 1.0f, 0.0f),	// Up Vector
		1.0f,							// near plane
		1000.0f);						// far plane
	switch (mode)
	{
	case PERSPECTIVE:
		
		break;
	case ORTHOGRAPHIC:
		m_cameraManager->SetProjectionType(ORTHOGRAPHIC);
		break;
	default:
		break;
	}
	
}

BulletOpenGLApplication::~BulletOpenGLApplication() {
	// Shutdown physics system
	ShutdownPhysics();
}

void BulletOpenGLApplication::SetScreenHeight(int screenHeight) {
	m_screenHeight = screenHeight;
}

void BulletOpenGLApplication::SetScreenWidth(int screenWidth) {
	m_screenWidth = screenWidth;
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

	// Disable Culling
	glDisable(GL_CULL_FACE);

	// set the backbuffer clearing color to a lightish blue
	glClearColor(0.6, 0.65, 0.85, 0);

	// initialize the physics system
	InitializePhysics();

	// create the debug drawer
	m_pDebugDrawer = new DebugDrawer();
	// set the initial debug level to 0
	m_pDebugDrawer->setDebugMode(0);
	// add the debug drawer to the world
	m_pWorld->setDebugDrawer(m_pDebugDrawer);

}

void BulletOpenGLApplication::Keyboard(unsigned char key, int x, int y) {
	// This function is called by FreeGLUT whenever
	// generic keys are pressed down.
	// Common to all projection types
	switch (key)
	{
	case 'v':
		// toggle wireframe debug drawing
		m_pDebugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
		break;
	case 'b':
		// toggle AABB debug drawing
		m_pDebugDrawer->ToggleDebugFlag(btIDebugDraw::DBG_DrawAabb);
		break;
	case 'z': m_cameraManager->ZoomCamera(+CAMERA_STEP_SIZE); break;			// 'z' zooms in
	case 'x': m_cameraManager->ZoomCamera(-CAMERA_STEP_SIZE); break;			// 'x' zoom out
	case 'w': m_cameraManager->TranslateCamera(UP, CAMERA_STEP_SIZE); break;
	case 'a': m_cameraManager->TranslateCamera(LEFT, CAMERA_STEP_SIZE); break;
	case 's': m_cameraManager->TranslateCamera(DOWN, -CAMERA_STEP_SIZE); break;
	case 'd': m_cameraManager->TranslateCamera(RIGHT, -CAMERA_STEP_SIZE); break;

	default:
		break;
	}
	// Switch for certain projection types
	switch (m_cameraManager->GetProjectionType())
	{
	case PERSPECTIVE:
		switch (key) {
		//
		}
		break;
	case ORTHOGRAPHIC:
		//
		break;
	default:
		break;
	}
	
}

void BulletOpenGLApplication::KeyboardUp(unsigned char key, int x, int y) {}

void BulletOpenGLApplication::Special(int key, int x, int y) {
	// This function is called by FreeGLUT whenever special keys
	// are pressed down, like the arrow keys, or Insert, Delete etc.
	printf("Received Special Key\n");

	switch (key) {
		// the arrow keys rotate the camera up/down/left/right
	case GLUT_KEY_LEFT:
		m_cameraManager->RotateCamera(YAW, +CAMERA_STEP_SIZE); break;
	case GLUT_KEY_RIGHT:
		m_cameraManager->RotateCamera(YAW, -CAMERA_STEP_SIZE); break;
	case GLUT_KEY_UP:
		m_cameraManager->RotateCamera(PITCH, +CAMERA_STEP_SIZE); break;
	case GLUT_KEY_DOWN:
		m_cameraManager->RotateCamera(PITCH, -CAMERA_STEP_SIZE); break;
	}

	switch (m_cameraManager->GetProjectionType())
	{
	case PERSPECTIVE:
		//
		break;
	case ORTHOGRAPHIC:
		//
		break;
	default:
		break;
	}
	
}

void BulletOpenGLApplication::SpecialUp(int key, int x, int y) {}

void BulletOpenGLApplication::Reshape(int w, int h) {
	printf("BulletOpenGLApplication Reshape called\n");
	// this function is called once during application intialization
	// and again every time we resize the window

	// set the viewport
	glViewport(0, 0, w, h);

	m_screenWidth = w;
	m_screenHeight = h;

	printf("screen width = %f, screen height = %f\n", m_screenWidth, m_screenHeight);

	// grab the screen width/height
	m_cameraManager->SetScreenWidth(w);
	m_cameraManager->SetScreenHeight(h);

	// update the camera
	m_cameraManager->UpdateCamera();
	//m_cameraManager->PrintCameraLocation();

	switch (m_cameraManager->GetProjectionType())	
	{
	case PERSPECTIVE:
		
		break;
	case ORTHOGRAPHIC:
		// 
		break;
	default:
		break;
	}

	
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

	m_cameraManager->UpdateCamera();

	switch (m_cameraManager->GetProjectionType())
	{
	case PERSPECTIVE:
		//
		break;
	case ORTHOGRAPHIC:
		break;
	default:
		break;
	}

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

	DrawWithTriangles(vertices, indices, 36);
}

void BulletOpenGLApplication::DrawPlane(const btVector3 &halfSize) {
	

	float halfWidth = halfSize.x();
	float halfHeight = halfSize.y();
	float halfDepth = halfSize.z(); // No depth

	// Create Vector
	btVector3 vertices[4] = {
		btVector3(-halfWidth, -halfHeight, 0.0f),
		btVector3(-halfWidth, halfHeight, 0.0f),
		btVector3(halfWidth, -halfHeight, 0.0f),
		btVector3(halfWidth, halfHeight, 0.0f),
	};

	if (m_cameraManager->GetProjectionType() == ORTHOGRAPHIC)
	{
		float pixels_to_meters = GetPixelsToMeters(0);
		// Build Vectors for normalization
		btVector3 m2Pvec(1 / pixels_to_meters, 1 / pixels_to_meters, 0.0f);
		btVector3 normalizeVec(1 / (m_screenWidth / 2), 1 / (m_screenHeight / 2), 0.0f);

		btVector3 multVec = m2Pvec * normalizeVec;

		// Normalize vertices between -1 and 1
		for (int index = 0; index < 4; index++) {

			btVector3 *vec = &vertices[index];
			*vec = *vec * multVec;

			printf("(x,y) = (%f,%f)\n", vec->getX(), vec->getY());

		}
	}

	// create the indexes for each triangle, using the 
	// vertices above. Make it static so we don't waste 
	// processing time recreating it over and over again
	static int indices[6] = {
		0, 1, 2,
		3, 2, 1
	};

	DrawWithTriangles(vertices, indices, 6);
}

void BulletOpenGLApplication::DrawWithTriangles(const btVector3 *vertices, const int *indices, int numberOfIndices) {

	// start processing vertices as triangles
	glBegin(GL_TRIANGLES);

	// increment the loop by 3 each time since we create a 
	// triangle with 3 vertices at a time.

	for (int i = 0; i < numberOfIndices; i+=3) {
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
	glEnd();
}


void BulletOpenGLApplication::RenderScene() {

	// create an array of 16 floats (representing a 4x4 matrix)
	btScalar transform[16];

	// iterate through all of the objects in our world
	//printf("number of objects = %d\n", m_objects.size());
	for (GameObjects::iterator i = m_objects.begin(); i != m_objects.end(); ++i) {
		// get the object from the iterator
		GameObject* pObj = *i;

		// read the transform
		pObj->GetTransform(transform);

		// get data from the object and draw it
		DrawShape(transform, pObj->GetShape(), pObj->GetColor());
	}

	// after rendering all game objects, perform debug rendering
	// Bullet will figure out what needs to be drawn then call to
	// our DebugDrawer class to do the rendering for us
	m_pWorld->debugDrawWorld();

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
	case BOX_2D_SHAPE_PROXYTYPE: {
		// assume the shape is a 2d box (plane) and typecast it
		const btBox2dShape *plane = static_cast<const btBox2dShape*> (pShape);
		btVector3 halfSize = plane->getHalfExtentsWithMargin();
		DrawPlane(halfSize);
	}
	default:
		// unsupported type
		break;
	}

	glPopMatrix();

}

float BulletOpenGLApplication::GetPixelsToMeters(float distanceToCamera) {

	return 0.00162*distanceToCamera + 0.0261;

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
	printf("Created Object and pushed to world\n");
	// check if the world object is valid
	if (m_pWorld) {
		// add the object's rigid body to the world
		m_pWorld->addRigidBody(pObject->GetRigidBody());

	}

	return pObject;

}