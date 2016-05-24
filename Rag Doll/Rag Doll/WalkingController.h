#pragma once
#include <vector>

class RagDollApplication;
class State;
class Gains;

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
	
	void SaveStates();
	void SaveGains();

	void Walk();
	void PauseWalking();
	void Reset();

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

private:

	RagDollApplication *m_app;
	

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

	float calculateTorqueForTorso(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForUpperLeftLeg(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForUpperRightLeg(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForLowerLeftLeg(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForLowerRightLeg(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForLeftFoot(float targetPosition, float currentPosition, float currentVelocity);
	float calculateTorqueForRightFoot(float targetPosition, float currentPosition, float currentVelocity);

	float calculateTorque(float kp, float kd, float targetPosition, float currentPosition, float velocity);


};
