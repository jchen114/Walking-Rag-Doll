#pragma once
#include "BulletOpenGLApplication.h"

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

};

