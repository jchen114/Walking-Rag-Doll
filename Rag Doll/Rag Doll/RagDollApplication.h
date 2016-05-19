#pragma once
#include "BulletOpenGLApplication.h"
#include <glui\glui.h>

class WalkingController;

class RagDollApplication :
	public BulletOpenGLApplication
{

public:
	RagDollApplication();
	RagDollApplication(ProjectionMode mode);
	~RagDollApplication();

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;

	void CreateRagDoll(const btVector3 &position);

	GameObject *Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);
	GameObject *Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);

	btVector3 GetRandomColor();

	void ApplyTorqueOnTorso(float torqueForce);
	void ApplyTorqueOnUpperRightLeg(float torqueForce);
	void ApplyTorqueOnUpperLeftLeg(float torqueForce);
	void ApplyTorqueOnLowerRightLeg(float torqueForce);
	void ApplyTorqueOnLowerLeftLeg(float torqueForce);
	void ApplyTorqueOnRightFoot(float torqueForce);
	void ApplyTorqueOnLeftFoot(float torqueForce);


private:

	WalkingController *m_WalkingController;

	// GLUI
	GLUI *m_glui_window;

	// Rag Doll model
	GameObject *m_torso;
	GameObject *m_upperRightLeg;
	GameObject *m_upperLeftLeg;
	GameObject *m_lowerRightLeg;
	GameObject *m_lowerLeftLeg;
	GameObject *m_rightFoot;
	GameObject *m_leftFoot;

	void ApplyTorqueOnGameBody(GameObject *body, float torqueForce);
	void CreateRagDollGUI();

	// GLUI CALLBACKS
	void CloseGLUIWindow(int id);

};

static void RagDollIdle();