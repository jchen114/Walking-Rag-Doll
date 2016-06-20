#pragma once
#include <vector>
#include <cstdio>
#include <ctime>

#include "LinearMath\btQuickprof.h"
#include "LinearMath\btVector3.h"

class RagDollApplication;
class State;
class Gains;


#pragma region DEFINITIONS 

#define PI 3.14159265

#define TORQUE_LIMIT 2000

// MASS
#define torso_mass 70
#define upper_leg_mass 5
#define lower_leg_mass 4
#define feet_mass 1

// DIMENSIONS. 
#define torso_height SCALING_FACTOR * 0.48f
#define torso_width torso_height / 5

#define upper_leg_height SCALING_FACTOR * 0.45f
#define upper_leg_width upper_leg_height / 5

#define lower_leg_height SCALING_FACTOR * 0.45f
#define lower_leg_width lower_leg_height / 7

#define foot_height SCALING_FACTOR * 0.05f
#define foot_width foot_height * 4

#define GROUND_WIDTH SCALING_FACTOR * 200.0f
#define GROUND_HEIGHT SCALING_FACTOR * 0.2f
#define MARKER_WIDTH SCALING_FACTOR * 0.03f
#define MARKER_HEIGHT SCALING_FACTOR * 0.05f
#define MARKER_DISTANCE SCALING_FACTOR * 2.0f

#define MARKER_COLOR btVector3(255,255,0)
#define DRAW_SPEEDUP 10

// Gains
#define KP_LOWER 0.0f
#define KP_HIGHER 5000.0f
#define KD_LOWER 0.0f
#define KD_HIGHER 5000.0f

// Spinner limits
#define SPINNER_TORSO_LOW		-90.0f
#define SPINNER_TORSO_HIGH		90.0f
#define SPINNER_UPPER_LEG_LOW	-90.0f
#define SPINNER_UPPER_LEG_HIGH	90.0f
#define SPINNER_LOWER_LEG_LOW	0.0f
#define SPINNER_LOWER_LEG_HIGH	150.0f
#define SPINNER_FOOT_LOW		-15.0f
#define SPINNER_FOOT_HIGH		90.0f

// Hinge limits
#define HINGE_TORSO_ULL_LOW		-90.0f
#define HINGE_TORSO_ULL_HIGH	90.0f

#define HINGE_TORSO_URL_LOW		-90.0f
#define HINGE_TORSO_URL_HIGH	90.0f

#define HINGE_ULL_LLL_LOW		0.0f
#define HINGE_ULL_LLL_HIGH		150.0f

#define HINGE_URL_LRL_LOW		0.0f
#define HINGE_URL_LRL_HIGH		150.0f

#define HINGE_LLL_LF_LOW		-15.0f
#define HINGE_LLL_LF_HIGH		90.0f

#define HINGE_LRL_RF_LOW		-15.0f
#define HINGE_LRL_RF_HIGH		90.0f

#define GROUND_POSITION btVector3(0, SCALING_FACTOR * -0.5, 0.0f)
#define ORIGINAL_TORSO_POSITION btVector3(0, (SCALING_FACTOR *-0.5 + GROUND_HEIGHT/2 + foot_height/2 + lower_leg_height + upper_leg_height + torso_height/2), 0)

#pragma endregion DEFINITIONS


enum CurrentRagDollState { STATE_0 = 100, STATE_1, STATE_2, STATE_3, STATE_4 };
enum CurrentControllerState {WALKING = 105, PAUSE, RESET};

class WalkingController
{
	
public:
	WalkingController();
	WalkingController(RagDollApplication *app);
	
	~WalkingController();

	std::vector<State *> ReadStateFile();
	std::vector<Gains *> ReadGainsFile();
	std::vector<float> ReadFeedbackFile();
	float ReadTimeFile();

	void SaveStates();
	void SaveGains();
	void SaveFeedback();
	void SaveTime();

	void StateLoop();
	void InitiateWalking();
	void PauseWalking();
	void Reset();
	void NotifyLeftFootGroundContact();
	void NotifyRightFootGroundContact();

	void SetState1(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot);
	void SetState2(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot);
	void SetState3(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot);
	void SetState4(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot);

	void SetTorsoGains(float kp, float kd);
	void SetUpperLeftLegGains(float kp, float kd);
	void SetUpperRightLegGains(float kp, float kd);
	void SetLowerLeftLegGains(float kp, float kd);
	void SetLowerRightLegGains(float kp, float kd);
	void SetLeftFootGains(float kp, float kd);
	void SetRightFootGains(float kp, float kd);

	CurrentControllerState m_currentState = RESET;
	CurrentRagDollState m_ragDollState = STATE_0;

	float m_cd_1 = 0.0f;
	float m_cv_1 = 0.0f;
	float m_cd_2 = 0.0f;
	float m_cv_2 = 0.0f;

	float m_state_time = 0.0f;

	btVector3 m_stanceAnklePosition = btVector3(0, 0, 0);
	btVector3 m_COMPosition = btVector3(0,0,0);

private:

	bool m_leftFootGroundHasContacted = false;
	bool m_rightFootGroundHasContacted = false;

	RagDollApplication *m_app;

	btClock m_clock;
	double m_duration = 0.0f;
	bool m_reset = true;

	// Set these in the GUI
	Gains *m_torso_gains;
	Gains *m_ull_gains;
	Gains *m_url_gains;
	Gains *m_lll_gains;
	Gains *m_lrl_gains;
	Gains *m_lf_gains;
	Gains *m_rf_gains;

	State *m_state0;
	State *m_state1;
	State *m_state2;
	State *m_state3;
	State *m_state4;

	std::vector<float> CalculateState1Torques();
	std::vector<float> CalculateState2Torques();
	std::vector<float> CalculateState3Torques();
	std::vector<float> CalculateState4Torques();

	float CalculateFeedbackSwingHip();

	float CalculateTorqueForTorso(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForUpperLeftLeg(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForUpperRightLeg(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForLowerLeftLeg(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForLowerRightLeg(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForLeftFoot(float targetPosition, float currentPosition, float currentVelocity);
	float CalculateTorqueForRightFoot(float targetPosition, float currentPosition, float currentVelocity);

	float CalculateTorque(float kp, float kd, float targetPosition, float currentPosition, float velocity);

};
