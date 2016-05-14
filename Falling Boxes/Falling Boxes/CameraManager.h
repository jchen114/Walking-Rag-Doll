#ifndef _CAMERAMANAGER_H_
#define _CAMERAMANAGER_H_

#include <Windows.h>

#include "BulletDynamics\Dynamics\btDynamicsWorld.h"
#include <gl\GL.h>
#include <freeglut\freeglut.h>

enum RotationType {YAW, PITCH, ROLL};
enum ProjectionType {ORTHOGRAPHIC, PERSPECTIVE};
enum TranslateDirection{UP, DOWN, LEFT, RIGHT};

class CameraManager
{
public:
	CameraManager(const btVector3 &target, float distance, float pitch, float yaw, const btVector3 &upVector, float nearPlane, float farPlane);
	~CameraManager();

	void SetScreenWidth(int screenWidth);
	void SetScreenHeight(int screenHeight);

	void UpdateCamera();
	void RotateCamera(RotationType type, float value);
	void ZoomCamera(float distance);
	void TranslateCamera(TranslateDirection direction, float value);
	void PrintCameraLocation();
	btVector3 GetCameraLocation();
	void SetProjectionType(ProjectionType type);
	ProjectionType GetProjectionType();


protected:
	
	int m_screenWidth;
	int m_screenHeight;

	void SetupPerspectiveCamera();
	void SetupOrthographicCamera();
	void SetupPerspectiveModelView();
	void SetupOrthographicModelView();


	btVector3 m_cameraPosition;
	btVector3 m_cameraTarget;
	float m_nearPlane;
	float m_farPlane;
	btVector3 m_upVector;
	float m_cameraDistance;
	float m_cameraPitch;
	float m_cameraYaw;

	float m_cameraPosX;
	float m_cameraPosY;

	ProjectionType m_projectionType;

};

#endif