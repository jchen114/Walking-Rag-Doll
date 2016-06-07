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
