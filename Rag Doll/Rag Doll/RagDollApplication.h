#pragma once
#include "BulletOpenGLApplication.h"
#include <glui\glui.h>
#include <vector>

class WalkingController;
class State;
class Gains;

class RagDollApplication :
	public BulletOpenGLApplication
{

public:
	RagDollApplication();
	RagDollApplication(ProjectionMode mode);
	~RagDollApplication();

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;
	virtual void Idle() override;

	void CreateRagDoll(const btVector3 &position);
	void AddHinges();

	GameObject *Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);
	GameObject *Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);

	btVector3 GetRandomColor();

	// Rag Doll model
	GameObject *m_torso;
	GameObject *m_upperRightLeg;
	GameObject *m_upperLeftLeg;
	GameObject *m_lowerRightLeg;
	GameObject *m_lowerLeftLeg;
	GameObject *m_rightFoot;
	GameObject *m_leftFoot;

	void Reset();
	void SaveStates();
	void SaveGains();
	void SaveFeedback();
	void SaveTime();
	void Start();
	void Pause();
	void CloseGLUIWindow(int id);
	void ChangeState(int id);
	void ChangeTorsoAngle();
	void ChangeUpperLeftLegAngle();
	void ChangeUpperRightLegAngle();
	void ChangeLowerLeftLegAngle();
	void ChangeLowerRightLegAngle();
	void ChangeLeftFootAngle();
	void ChangeRightFootAngle();

	void ApplyTorqueOnTorso(float torqueForce);
	void ApplyTorqueOnUpperRightLeg(float torqueForce);
	void ApplyTorqueOnUpperLeftLeg(float torqueForce);
	void ApplyTorqueOnLowerRightLeg(float torqueForce);
	void ApplyTorqueOnLowerLeftLeg(float torqueForce);
	void ApplyTorqueOnRightFoot(float torqueForce);
	void ApplyTorqueOnLeftFoot(float torqueForce);
	void RagDollStep();

private:

	void DisplayState(int state);
	void DisplayGains();
	void DisplayFeedback(std::vector<float> feedbacks);
	void DisplayTime(float time);
	void DisableStateSpinner();
	void DisableAllSpinners();
	void EnableGainSpinners();
	void UpdateRagDoll();
	void UpdateGains();

	WalkingController *m_WalkingController;

	// GLUI
	GLUI *m_glui_window;
	int m_currentState = 0;
	int m_previousState = 0;

	std::vector<GameObject *> m_bodies;

	btHingeConstraint *m_torso_urLeg;
	btHingeConstraint *m_torso_ulLeg;
	btHingeConstraint *m_urLeg_lrLeg;
	btHingeConstraint *m_ulLeg_llLeg;
	btHingeConstraint *m_lrLeg_rFoot;
	btHingeConstraint *m_llLeg_lFoot;

	// States
	std::vector<State *> m_states;
	std::vector<Gains *> m_gains;

	void ApplyTorqueOnGameBody(GameObject *body, float torqueForce);
	void CreateRagDollGUI();
	void SetupGUIConfiguration(std::vector<State *>states, std::vector<Gains *> gains);

	// GLUI Members

	GLUI_Spinner *m_torso_kp_spinner;
	GLUI_Spinner *m_torso_kd_spinner;

	GLUI_Spinner *m_ull_kp_spinner;
	GLUI_Spinner *m_ull_kd_spinner;

	GLUI_Spinner *m_url_kp_spinner;
	GLUI_Spinner *m_url_kd_spinner;

	GLUI_Spinner *m_lll_kp_spinner;
	GLUI_Spinner *m_lll_kd_spinner;

	GLUI_Spinner *m_lrl_kp_spinner;
	GLUI_Spinner *m_lrl_kd_spinner;

	GLUI_Spinner *m_lf_kp_spinner;
	GLUI_Spinner *m_lf_kd_spinner;

	GLUI_Spinner *m_rf_kp_spinner;
	GLUI_Spinner *m_rf_kd_spinner;

	GLUI_RadioGroup *m_StatesRadioGroup;
	GLUI_Spinner *m_torso_state_spinner;
	GLUI_Spinner *m_ull_state_spinner;
	GLUI_Spinner *m_url_state_spinner;
	GLUI_Spinner *m_lll_state_spinner;
	GLUI_Spinner *m_lrl_state_spinner;
	GLUI_Spinner *m_lf_state_spinner;
	GLUI_Spinner *m_rf_state_spinner;

	GLUI_Spinner *m_cd_1_spinner;
	GLUI_Spinner *m_cv_1_spinner;
	GLUI_Spinner *m_cd_2_spinner;
	GLUI_Spinner *m_cv_2_spinner;

	GLUI_Spinner *m_timer_spinner;

};

/* GLUI CALLBACKS */
static void RagDollIdle();
static void SaveGainsButtonPressed(int id);
static void SaveStatesButtonPressed(int id);
static void SaveFeedbackButtonPressed(int id);
static void SaveTimeButtonPressed(int id);
static void ResetButtonPressed(int id);
static void PauseButtonPressed(int id);
static void StartButtonPressed(int id);
static void StateChanged(int id);

static void TorsoAngleChanged(int id);
static void UpperLeftLegAngleChanged(int id);
static void UpperRightLegAngleChanged(int id);
static void LowerLeftLegAngleChanged(int id);
static void LowerRightLegAngleChanged(int id);
static void LeftFootAngleChanged(int id);
static void RightFootAngleChanged(int id);
static void GainsChanged(int id);

/* Bullet Physics Tick callback */
void InternalTickCallback(btDynamicsWorld *world, btScalar timestep);