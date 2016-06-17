#pragma once
#include "BulletOpenGLApplication.h"
#include <glui\glui.h>
#include <vector>

class WalkingController;
class State;
class Gains;

#define MARKER_DISTANCE 2.0f
#define GROUND_WIDTH 200.0f
#define MARKER_WIDTH 0.03f
#define MARKER_HEIGHT 0.09f
#define MARKER_COLOR btVector3(255,255,0)
#define DRAW_SPEEDUP 10

class RagDollApplication :
	public BulletOpenGLApplication
{

public:
	RagDollApplication();
	RagDollApplication(ProjectionMode mode, bool isFrameRateFixed = false);
	~RagDollApplication();

	virtual void InitializePhysics() override;
	virtual void ShutdownPhysics() override;
	virtual void Keyboard(unsigned char key, int x, int y) override;
	virtual void KeyboardUp(unsigned char key, int x, int y) override;
	virtual void DrawShape(btScalar *transform, const btCollisionShape *pShape, const btVector3 &color) override;
	void DrawTorso(const btVector3 &halfSize);
	void DrawUpperLeg(const btVector3 &halfSize);
	void DrawLowerLeg(const btVector3 &halfSize);
	void DrawFoot(const btVector3 &halfSize);

	virtual void Idle() override;

	void CreateGround(const btVector3 &position);
	void CreateRagDoll(const btVector3 &position);
	void AddHinges();

	GameObject *Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);
	GameObject *Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position);
	GameObject *Create2DCircle(float radius, float mass, const btVector3 &color, const btVector3 &position);
	GameObject *Create2DLine(const btVector3 &start, const btVector3 &end, const btVector3 &color);

	btVector3 GetRandomColor();

	GameObject *m_ground;

	// Rag Doll model
	GameObject *m_torso;
	GameObject *m_upperRightLeg;
	GameObject *m_upperLeftLeg;
	GameObject *m_lowerRightLeg;
	GameObject *m_lowerLeftLeg;
	GameObject *m_rightFoot;
	GameObject *m_leftFoot;
	// For debugging
	GameObject *m_stanceAnkle;
	GameObject *m_COM;
	GameObject *m_distance;

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

	void DrawArrow(const btVector3 &pointOfContact, TranslateDirection direction);

	void ApplyTorqueOnUpperRightLeg(float torqueForce);
	void ApplyTorqueOnUpperLeftLeg(float torqueForce);
	void ApplyTorqueOnLowerRightLeg(float torqueForce);
	void ApplyTorqueOnLowerLeftLeg(float torqueForce);
	void ApplyTorqueOnRightFoot(float torqueForce);
	void ApplyTorqueOnLeftFoot(float torqueForce);
	void RagDollStep();
	void RagDollCollision();

private:

	void DrawDebugFeedback();

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

	int m_draw = 0;
	char buf[1000];
	btVector3 m_color;

	btVector3 m_previous_torso_position;

	bool m_drawBackForce = false;
	bool m_drawForwardForce = false;

};

static void DrawFilledCircle(GLfloat x, GLfloat y, GLfloat radius, const btVector3 &color = btVector3(255, 255, 255));
static void DrawPartialFilledCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat begin, GLfloat end); // Draw CCW from begin to end
static void DrawLine(const btVector3 &begin, const btVector3 &end, const btVector3 &color);

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
void InternalPreTickCallback(btDynamicsWorld *world, btScalar timestep);
void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep);