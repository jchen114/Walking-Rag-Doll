#pragma once


enum ProjectionMode{ ORTHOGRAPHIC, PERSPECTIVE };
enum Dimension{ HEIGHT, WIDTH };

#define CAMERA_STEP_SIZE 20.0f
#define Z_PLANE  0

#define DEG_2_RAD 0.0174533

class Constants
{
	
private:

	ProjectionMode m_projectionMode;
	int m_screenWidth;
	int m_screenHeight;

public:

	static Constants& GetInstance() {
		static Constants instance;
		return instance;
	}

	void SetScreenWidth(int width);
	void SetScreenHeight(int height);
	void SetProjectionMode(ProjectionMode mode);

	int GetScreenWidth();
	int GetScreenHeight();

	ProjectionMode GetProjectionMode();

	float GetMetersToPixels(float dist2Camera);

	float DegreesToRadians(float degrees);
	float RadiansToDegrees(float radians);

protected:

	Constants();
	~Constants();
};

