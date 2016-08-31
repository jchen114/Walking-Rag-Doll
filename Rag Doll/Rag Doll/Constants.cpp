#include "stdafx.h"
#include "Constants.h"


Constants::Constants()
{
}

void Constants::SetScreenWidth(int width) {
	m_screenWidth = width;
}

void Constants::SetScreenHeight(int height) {
	m_screenHeight = height;
}


void Constants::SetProjectionMode(ProjectionMode mode) {
	m_projectionMode = mode;
}

int Constants::GetScreenWidth() {
	return m_screenWidth;
}

int Constants::GetScreenHeight() {
	return m_screenHeight;
}

ProjectionMode Constants::GetProjectionMode() {
	return m_projectionMode;
}

float Constants::GetMetersToPixels(float dist2Camera) {
	return 50 / dist2Camera;
}

float Constants::DegreesToRadians(float degrees) {

	return degrees * DEG_2_RAD;

}

float Constants::RadiansToDegrees(float radians) {
	return radians * (1 / DEG_2_RAD);
}

Constants::~Constants()
{
}

void DrawCircle(const float &radius) {

	int triangleAmount = 20; //# of triangles used to draw circle
	//GLfloat radius = 0.8f; //radius
	GLfloat twicePi = 2.0f * PI;

	glBegin(GL_TRIANGLE_FAN);

	for (int i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			(radius * cos(i *  twicePi / triangleAmount)),
			(radius * sin(i * twicePi / triangleAmount))
			);
	}
	glEnd();
}

void DisplayText(float x, float y, const btVector3 &color, const char *string) {
	int j = strlen(string);

	glColor3f(color.x(), color.y(), color.z());
	glRasterPos2f(x, y);
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
	}
}