#include "stdafx.h"

#include "WalkingController.h"
#include "RagDollApplication.h"

WalkingController::WalkingController()
{
}

WalkingController::WalkingController(RagDollApplication *app) {
	m_app = app;
}


WalkingController::~WalkingController()
{
}

#pragma region WALKER_INTERACTION

void WalkingController::StartWalking(){}
void WalkingController::PauseWalking(){}
void WalkingController::Reset(){}

#pragma endregion WALKER_INTERACTION

#pragma region GAINS

void WalkingController::SetTorsoGains(float kp, float kd){}
void WalkingController::SetUpperLeftLegGains(float kp, float kd){}
void WalkingController::SetUpperRightLegGains(float kp, float kd){}
void WalkingController::SetLowerLeftLegGains(float kp, float kd){}
void WalkingController::SetLowerRightLegGains(float kp, float kd){}
void WalkingController::SetLeftFootGains(float kp, float kd){}
void WalkingController::SetRightFootGains(float kp, float kd){}

#pragma endregion GAINS

#pragma region STATES

void WalkingController::SetState1(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){}
void WalkingController::SetState2(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){}
void WalkingController::SetState3(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){}
void WalkingController::SetState4(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){}

#pragma endregion STATES



