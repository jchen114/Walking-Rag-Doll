#pragma once

class RagDollApplication;

class WalkingController
{
	class State;

public:
	WalkingController();
	WalkingController(RagDollApplication *app);
	~WalkingController();

	void StartWalking();
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


private:

	RagDollApplication *m_app;

	// Set these in the GUI
	float m_kp_torso_hip;
	float m_kd_torso_hip;

	float m_kp_upper_left_leg_hip;
	float m_kd_upper_left_leg_hip;

	float m_kp_upper_right_leg_hip;
	float m_kd_upper_right_leg_hip;

	float m_kp_lower_left_leg_upper;
	float m_kd_lower_left_leg_upper;

	float m_kp_lower_right_leg_upper;
	float m_kd_lower_right_leg_upper;

	float m_kp_left_foot;
	float m_kd_left_foot;

	float m_kp_right_foot;
	float m_kd_right_foot;

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

class State {

public:

	State(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot) {
		m_torsoAngle = torso;
		m_upperLeftLegAngle = upperLeftLeg;
		m_upperRightLegAngle = upperRightLeg;
		m_lowerLeftLegAngle = lowerLeftLeg;
		m_lowerRightLegAngle = lowerRightLeg;
		m_leftFootAngle = leftFoot;
		m_rightFootAngle = rightFoot;
	}

	float m_torsoAngle;
	float m_upperRightLegAngle;
	float m_upperLeftLegAngle;
	float m_lowerRightLegAngle;
	float m_lowerLeftLegAngle;
	float m_rightFootAngle;
	float m_leftFootAngle;

};
