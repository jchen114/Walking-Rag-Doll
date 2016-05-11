#ifndef _CAMERAMANAGER_H_
#define _CAMERAMANAGER_H_

#include <Windows.h>

#include "BulletDynamics\Dynamics\btDynamicsWorld.h"
#include <gl\GL.h>
#include <freeglut\freeglut.h>

enum RotationType {YAW, PITCH, ROLL};

class CameraManager
{
public:
	CameraManager(const btVector3 &position, const btVector3 &target, float distance, float pitch, float yaw, const btVector3 &upVector, float nearPlane, float farPlane);
	~CameraManager();

	void setScreenWidth(int screenWidth);
	void setScreenHeight(int screenHeight);

	void UpdateCamera();
	void RotateCamera(RotationType type, float value);
	void ZoomCamera(float distance);

protected:
	
	int m_screenWidth;
	int m_screenHeight;

	btVector3 m_cameraPosition;
	btVector3 m_cameraTarget;
	float m_nearPlane;
	float m_farPlane;
	btVector3 m_upVector;
	float m_cameraDistance;
	float m_cameraPitch;
	float m_cameraYaw;

};

#endif