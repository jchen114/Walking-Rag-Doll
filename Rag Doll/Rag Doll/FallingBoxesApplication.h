#pragma once
#include "BulletOpenGLApplication.h"
class FallingBoxesApplication :
	public BulletOpenGLApplication
{
public:
	FallingBoxesApplication();
	FallingBoxesApplication(ProjectionMode mode);
	~FallingBoxesApplication();

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;

	virtual void Mouse(int button, int state, int x, int y) override;

	void Create2DBoxes();

	void CreateBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);

};

