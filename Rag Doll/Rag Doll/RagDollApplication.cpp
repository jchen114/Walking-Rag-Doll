#include "stdafx.h"
#include "RagDollApplication.h"
#include "WalkingController.h"

#include "State.h"	
#include "Gains.h"

#include <iostream>

using namespace std::placeholders;

#pragma region INITIALIZATION

static RagDollApplication *m_app;

enum ControlIDs { 
	TORSO_KP, TORSO_KD = 0, 
	UPPER_R_LEG_KP, UPPER_R_LEG_KD, 
	UPPER_L_LEG_KP, UPPER_L_LEG_KD, 
	LOWER_R_LEG_KP, LOWER_R_LEG_KD, 
	LOWER_L_LEG_KP, LOWER_L_LEG_KD, 
	R_FOOT_KP, R_FOOT_KD, 
	L_FOOT_KP, L_FOOT_KD };

enum StateIDs {
	TORSO_ANGLE = 13,
	ULL_ANGLE,
	URL_ANGLE,
	LLL_ANGLE,
	LRL_ANGLE,
	LF_ANGLE,
	RF_ANGLE
};

enum Button_IDs {
	RESET_BUTTON = 20,
	PAUSE_BUTTON,
	START_BUTTON,
	SAVESTATES_BUTTON,
	SAVEGAINS_BUTTON,
	SAVEFEEDBACK_BUTTON,
	SAVETIME_BUTTON
};

RagDollApplication::RagDollApplication()
{
}

RagDollApplication::RagDollApplication(ProjectionMode mode, bool isFrameRateFixed) :BulletOpenGLApplication(mode, isFrameRateFixed){
	m_app = this;
	m_DrawCallback = std::bind(&RagDollApplication::DrawDebugFeedback, this);
	m_DrawShapeCallback = std::bind(&RagDollApplication::DrawShape, this, _1, _2, _3);
}

RagDollApplication::~RagDollApplication()
{
}

void RagDollApplication::InitializePhysics() {

	// create the collision configuration
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	// Adding for 2D collisions and solving
	m_pDispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, new btBox2dBox2dCollisionAlgorithm::CreateFunc());

	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	m_pWorld->setGravity(btVector3(0.0f, SCALING_FACTOR * -9.81f,0.0f));

	// Create ground
	CreateGround(GROUND_POSITION);

	CreateRagDoll(ORIGINAL_TORSO_POSITION);

	// Create Controller
	m_WalkingController = new WalkingController(this);

	std::vector<std::string> gaits = m_WalkingController->GetGaits();
	int ind = 0;
	for (std::vector<std::string>::iterator it = gaits.begin(); it != gaits.end(); ++it) {
		std::string gait_name = *it;
		m_gaits.push_back(gait_name);
	}
	m_WalkingController->ChangeGait("Walk"); // Assume Walk gait exists

	m_previousState = 0;

	// Create GUI
	CreateRagDollGUI();

	// Setup GUI with configurations
	SetupGUIConfiguration();
	
	Reset();

	GameObject::DisableDeactivation(m_bodies);

	m_pWorld->setInternalTickCallback(InternalPreTickCallback, 0, true);
	m_pWorld->setInternalTickCallback(InternalPostTickCallback, 0, false);

	m_previous_torso_position = m_WalkingController->m_COMPosition;

}

void RagDollApplication::Idle() {
	BulletOpenGLApplication::Idle();
}

void RagDollApplication::Keyboard(unsigned char key, int x, int y) {
	BulletOpenGLApplication::Keyboard(key, x, y);
	
	switch (key)
	{
	case 'f':
	{
		//printf("pressed f key \n");
		
		m_drawBackForce = true;
		m_drawForwardForce = false;
		
	}
		break;
	case 'g': {
		
		m_drawForwardForce = true;
		m_drawBackForce = false;
	}
		break;
	default:
		break;
	}
	
}

void RagDollApplication::KeyboardUp(unsigned char key, int x, int y) {
	BulletOpenGLApplication::KeyboardUp(key, x, y);

	switch (key)
	{
	case 'f':
	{
		m_drawBackForce = false;
	}
		break;
	case 'g': {
		m_drawForwardForce = false;
	}
		break;

	default:
		break;
	}

}

void RagDollApplication::CreateGround(const btVector3 &position) {

	// Create ground.
	btVector3 ground(GROUND_WIDTH/2, GROUND_HEIGHT/2, 10.0f);
	float mass = 0.0f;
	m_ground = Create3DBox(ground, mass, GetRandomColor(), position);
	m_ground->GetRigidBody()->setCollisionFlags(m_ground->GetRigidBody()->getCollisionFlags());

	// Create markers.
	for (int marker = 0; marker < GROUND_WIDTH; marker += MARKER_DISTANCE) {
		// Position for marker
		btVector3 pos(marker - (GROUND_WIDTH / 2 - MARKER_WIDTH/2), position.getY() + (GROUND_HEIGHT/2 + MARKER_HEIGHT/2), 0);
		GameObject *box = Create2DBox(btVector3(MARKER_WIDTH / 2, MARKER_HEIGHT / 2, 0), 0, MARKER_COLOR, pos);
		// Create fixed constraint
		AddFixedConstraint(box, m_ground);
	}

}

void RagDollApplication::RagDollStep() {
	//printf("Callback before every physics tick.\n");
	
	switch (m_WalkingController->m_currentState)
	{
	case RESET:
	case WALKING: {
		btVector3 force;
		btVector3 relPos;
		if (m_drawForwardForce && !m_drawBackForce) {
			force = btVector3(SCALING_FACTOR * 0.5f, 0.0f, 0.0f);
			relPos = btVector3(0.0f, 0.0f, 0.0f);
			m_torso->GetRigidBody()->applyImpulse(force, relPos);
		}
		if (m_drawBackForce && !m_drawForwardForce) {
			force = btVector3(SCALING_FACTOR * -0.5f, 0.0f, 0.0f);
			relPos = btVector3(0.0f, 0.0f, 0.0f);
			m_torso->GetRigidBody()->applyImpulse(force, relPos);
		}
	}
		break;
	case PAUSE:
		break;
	default:
		break;
	}

	m_WalkingController->StateLoop();

}

void RagDollApplication::RagDollCollision() {

	if (m_collisionClock.getTimeMilliseconds() >= 90) {

		int numManifolds = m_pWorld->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = m_pWorld->getDispatcher()->getManifoldByIndexInternal(i);

			btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
			btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());

			for (int j = 0; j < contactManifold->getNumContacts(); j++)   {
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.m_distance1 < 0) {
					// Valid contact point
					if ((obA->getUserPointer() == m_leftFoot && obB->getUserPointer() == m_ground) || (obA->getUserPointer() == m_ground && obB->getUserPointer() == m_leftFoot)) {
						//printf(">>>>>>>>>>>>>>>>>>>>>> Collision with left foot to ground detected. <<<<<<<<<<<<<<<<<<<<<< \n");
						m_WalkingController->NotifyLeftFootGroundContact();
						break;
					}

					if ((obA->getUserPointer() == m_rightFoot && obB->getUserPointer() == m_ground) || (obA->getUserPointer() == m_ground && obB->getUserPointer() == m_rightFoot)) {
						//printf(">>>>>>>>>>>>>>>>>>>>>> Collision with right foot to ground detected. <<<<<<<<<<<<<<<<<<<<<< \n");
						m_WalkingController->NotifyRightFootGroundContact();
						break;
					}
					if ((obA->getUserPointer() == m_torso && obB->getUserPointer() == m_ground) || (obA->getUserPointer() == m_ground && obB->getUserPointer() == m_torso)) {
						//printf(">>>>>>>>>>>>>>>>>>>>>> Collision with right foot to ground detected. <<<<<<<<<<<<<<<<<<<<<< \n");
						m_WalkingController->NotifyTorsoGroundContact();
						break;
					}
				}
			}

		}
		m_collisionClock.reset();
	}
}

void RagDollApplication::ShutdownPhysics() {

}


#pragma region GUI

void RagDollApplication::CreateRagDollGUI() {

	Debug("Create Glui Window");

	// Setup
	GLUI_Master.set_glutIdleFunc(RagDollIdle);
	m_glui_window = GLUI_Master.create_glui("Rag Doll Controls", 5, 0);
	m_glui_window->set_main_gfx_window(m_main_window_id);

	// Controls
	/*===================================== GAINS =========================================*/
	GLUI_Panel *gains_panel = m_glui_window->add_panel("Gains");
	m_glui_window->add_statictext_to_panel(gains_panel, "Torso");
	m_torso_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, TORSO_KP, (GLUI_Update_CB) GainsChanged);
	m_torso_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, TORSO_KD, (GLUI_Update_CB)GainsChanged);

	m_torso_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_torso_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Left Leg");
	m_ull_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, UPPER_L_LEG_KP, (GLUI_Update_CB)GainsChanged);
	m_ull_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, UPPER_L_LEG_KD, (GLUI_Update_CB)GainsChanged);

	m_ull_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_ull_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Right Leg");
	m_url_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KP, (GLUI_Update_CB)GainsChanged);
	m_url_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KD, (GLUI_Update_CB)GainsChanged);

	m_url_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_url_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Left Leg");
	m_lll_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, LOWER_L_LEG_KP, (GLUI_Update_CB)GainsChanged);
	m_lll_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, LOWER_L_LEG_KD, (GLUI_Update_CB)GainsChanged);

	m_lll_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lll_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Right Leg");
	m_lrl_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, LOWER_R_LEG_KP, (GLUI_Update_CB) GainsChanged);
	m_lrl_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, LOWER_R_LEG_KD, (GLUI_Update_CB) GainsChanged);

	m_lrl_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lrl_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Left Foot");
	m_lf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, L_FOOT_KP, (GLUI_Update_CB) GainsChanged);
	m_lf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, L_FOOT_KD, (GLUI_Update_CB) GainsChanged);

	m_lf_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lf_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Right foot");
	m_rf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, R_FOOT_KP, (GLUI_Update_CB) GainsChanged);
	m_rf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, R_FOOT_KD, (GLUI_Update_CB) GainsChanged);

	m_rf_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_rf_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_button_to_panel(gains_panel, "Save Gains", SAVEGAINS_BUTTON, (GLUI_Update_CB)SaveGainsButtonPressed);

	// Vertical separation
	m_glui_window->add_column(true);

	/*===================================== STATES =========================================*/
	GLUI_Panel *states_panel = m_glui_window->add_panel("States");
	m_StatesRadioGroup = m_glui_window->add_radiogroup_to_panel(states_panel, &m_currentState, -1, (GLUI_Update_CB)StateChanged);

	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 0");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 1");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 2");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 3");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 4");

	m_glui_window->add_separator_to_panel(states_panel);

	m_glui_window->add_statictext_to_panel(states_panel, "Desired State Angles");

	m_torso_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Torso Angle", GLUI_SPINNER_FLOAT, NULL, TORSO_ANGLE, (GLUI_Update_CB)TorsoAngleChanged);
	m_ull_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Upper left leg Angle", GLUI_SPINNER_FLOAT, NULL, ULL_ANGLE, (GLUI_Update_CB)UpperLeftLegAngleChanged);
	m_url_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Upper right leg Angle", GLUI_SPINNER_FLOAT, NULL, URL_ANGLE, (GLUI_Update_CB)UpperRightLegAngleChanged);
	m_lll_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Lower left leg Angle", GLUI_SPINNER_FLOAT, NULL, LLL_ANGLE, (GLUI_Update_CB)LowerLeftLegAngleChanged);
	m_lrl_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Lower right leg Angle", GLUI_SPINNER_FLOAT, NULL, LRL_ANGLE, (GLUI_Update_CB)LowerRightLegAngleChanged);
	m_lf_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Left foot Angle", GLUI_SPINNER_FLOAT, NULL, LF_ANGLE, (GLUI_Update_CB)LeftFootAngleChanged);
	m_rf_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Right foot Angle", GLUI_SPINNER_FLOAT, NULL, RF_ANGLE, (GLUI_Update_CB)RightFootAngleChanged);

	m_torso_state_spinner->set_float_limits(SPINNER_TORSO_LOW, SPINNER_TORSO_HIGH);
	m_ull_state_spinner->set_float_limits(SPINNER_UPPER_LEG_LOW, SPINNER_UPPER_LEG_HIGH);
	m_url_state_spinner->set_float_limits(SPINNER_UPPER_LEG_LOW, SPINNER_UPPER_LEG_HIGH);
	m_lll_state_spinner->set_float_limits(SPINNER_LOWER_LEG_LOW, SPINNER_LOWER_LEG_HIGH);
	m_lrl_state_spinner->set_float_limits(SPINNER_LOWER_LEG_LOW, SPINNER_LOWER_LEG_HIGH);
	m_lf_state_spinner->set_float_limits(SPINNER_FOOT_LOW, SPINNER_FOOT_HIGH);
	m_rf_state_spinner->set_float_limits(SPINNER_FOOT_LOW, SPINNER_FOOT_HIGH);

	m_glui_window->add_button_to_panel(states_panel, "Save States", SAVESTATES_BUTTON, (GLUI_Update_CB)SaveStatesButtonPressed);

	/*===================================== FEEDBACK =========================================*/
	GLUI_Panel *feedback_panel = m_glui_window->add_panel("Feedback");
	m_cd_1_spinner = m_glui_window->add_spinner_to_panel(feedback_panel, "cd 1", GLUI_SPINNER_FLOAT, NULL, -1, (GLUI_Update_CB) FeedbackChanged);
	m_cv_1_spinner = m_glui_window->add_spinner_to_panel(feedback_panel, "cv 1", GLUI_SPINNER_FLOAT, NULL, -1, (GLUI_Update_CB) FeedbackChanged);
	m_cd_2_spinner = m_glui_window->add_spinner_to_panel(feedback_panel, "cd 2", GLUI_SPINNER_FLOAT, NULL, -1, (GLUI_Update_CB) FeedbackChanged);
	m_cv_2_spinner = m_glui_window->add_spinner_to_panel(feedback_panel, "cv 2", GLUI_SPINNER_FLOAT, NULL, -1, (GLUI_Update_CB) FeedbackChanged);
	m_glui_window->add_button_to_panel(feedback_panel, "Save Feedback", SAVEFEEDBACK_BUTTON, (GLUI_Update_CB)SaveFeedbackButtonPressed);

	/*===================================== STATE TIME ============================================*/
	GLUI_Panel *time_panel = m_glui_window->add_panel("State time");
	m_timer_spinner = m_glui_window->add_spinner_to_panel(time_panel, "time", GLUI_SPINNER_FLOAT, &m_WalkingController->m_state_time);
	m_glui_window->add_button_to_panel(time_panel, "Save Time", SAVETIME_BUTTON, (GLUI_Update_CB)SaveTimeButtonPressed);

	/*===================================== CONTROLS =========================================*/

	GLUI_Panel *control_panel = m_glui_window->add_panel("Controls");
	m_glui_window->add_button_to_panel(control_panel, "Reset", RESET_BUTTON, (GLUI_Update_CB)ResetButtonPressed);
	m_glui_window->add_separator_to_panel(control_panel);
	m_glui_window->add_button_to_panel(control_panel, "Pause", PAUSE_BUTTON, (GLUI_Update_CB)PauseButtonPressed);
	m_glui_window->add_separator_to_panel(control_panel);
	m_glui_window->add_button_to_panel(control_panel, "Start", START_BUTTON, (GLUI_Update_CB)StartButtonPressed);

	//// Vertical separation
	m_glui_window->add_column(true);

	GLUI_Panel *gait_panel = m_glui_window->add_panel("Gaits");
	m_GaitsRadioGroup = m_glui_window->add_radiogroup_to_panel(gait_panel, &m_currentGait, -1, (GLUI_Update_CB)GaitsChanged);
	// Determine how many gaits we have.
	int walk_index = 0;
	int ind = 0;
	for (std::vector<std::string>::iterator it = m_gaits.begin(); it != m_gaits.end(); ++it) {
		std::string gait_name = *it;
		m_glui_window->add_radiobutton_to_group(m_GaitsRadioGroup, gait_name.c_str());
		if (strcmp(gait_name.c_str(), "Walk") == 0) {
			walk_index = ind;
		}
		ind++;
	}
	m_GaitsRadioGroup->set_int_val(walk_index);
	m_previousGait = walk_index;
	m_currentGait = walk_index;

}

void RagDollApplication::SetupGUIConfiguration() {

	// Assume Currently Selected State is 0
	DisplayGains();

	DisplayFeedback();
	DisplayTime();

	DisableStateSpinner();

}

void RagDollApplication::DisplayState(int state) {

	State *selected_state = GetState(state);

	m_torso_state_spinner->set_float_val(selected_state->m_torsoAngle);
	
	m_ull_state_spinner->set_float_val(selected_state->m_upperLeftLegAngle);
	m_url_state_spinner->set_float_val(selected_state->m_upperRightLegAngle);
	
	// Lower legs have to relative orientation to upper legs. Local coordinates
	m_lll_state_spinner->set_float_val(selected_state->m_lowerLeftLegAngle);
	m_lrl_state_spinner->set_float_val(selected_state->m_lowerRightLegAngle);
	
	m_lf_state_spinner->set_float_val(selected_state->m_leftFootAngle);
	m_rf_state_spinner->set_float_val(selected_state->m_rightFootAngle);

}

void RagDollApplication::DisplayGains() {

		m_torso_kp_spinner->set_float_val(m_WalkingController->m_torso_gains->m_kp);
		m_torso_kd_spinner->set_float_val(m_WalkingController->m_torso_gains->m_kd);

		m_ull_kp_spinner->set_float_val(m_WalkingController->m_ull_gains->m_kp);
		m_ull_kd_spinner->set_float_val(m_WalkingController->m_ull_gains->m_kd);

		m_url_kp_spinner->set_float_val(m_WalkingController->m_url_gains->m_kp);
		m_url_kd_spinner->set_float_val(m_WalkingController->m_url_gains->m_kd);

		m_lll_kp_spinner->set_float_val(m_WalkingController->m_lll_gains->m_kp);
		m_lll_kd_spinner->set_float_val(m_WalkingController->m_lll_gains->m_kd);

		m_lrl_kp_spinner->set_float_val(m_WalkingController->m_lrl_gains->m_kp);
		m_lrl_kd_spinner->set_float_val(m_WalkingController->m_lrl_gains->m_kd);

		m_lf_kp_spinner->set_float_val(m_WalkingController->m_lf_gains->m_kp);
		m_lf_kd_spinner->set_float_val(m_WalkingController->m_lf_gains->m_kd);

		m_rf_kp_spinner->set_float_val(m_WalkingController->m_rf_gains->m_kp);
		m_rf_kd_spinner->set_float_val(m_WalkingController->m_rf_gains->m_kd);


}

void RagDollApplication::DisplayFeedback() {

	m_cd_1_spinner->set_float_val(*m_WalkingController->m_cd_1);
	m_cv_1_spinner->set_float_val(*m_WalkingController->m_cv_1);
	m_cd_2_spinner->set_float_val(*m_WalkingController->m_cd_2);
	m_cv_2_spinner->set_float_val(*m_WalkingController->m_cv_2);

}

void RagDollApplication::DisplayTime() {

	m_timer_spinner->set_float_val(m_WalkingController->m_state_time);

}

void RagDollApplication::UpdateGains() {
	m_WalkingController->SetTorsoGains(m_torso_kp_spinner->get_float_val(), m_torso_kd_spinner->get_float_val());
	m_WalkingController->SetUpperLeftLegGains(m_ull_kp_spinner->get_float_val(), m_ull_kd_spinner->get_float_val());
	m_WalkingController->SetUpperRightLegGains(m_url_kp_spinner->get_float_val(), m_url_kd_spinner->get_float_val());
	m_WalkingController->SetLowerLeftLegGains(m_lll_kp_spinner->get_float_val(), m_lll_kd_spinner->get_float_val());
	m_WalkingController->SetLowerRightLegGains(m_lrl_kp_spinner->get_float_val(), m_lrl_kd_spinner->get_float_val());
	m_WalkingController->SetLeftFootGains(m_lf_kp_spinner->get_float_val(), m_lf_kd_spinner->get_float_val());
	m_WalkingController->SetRightFootGains(m_rf_kp_spinner->get_float_val(), m_rf_kd_spinner->get_float_val());
}

void RagDollApplication::UpdateFeedbacks() {
	
	m_WalkingController->SetFeedback1(m_cd_1_spinner->get_float_val(), m_cv_1_spinner->get_float_val());
	m_WalkingController->SetFeedback2(m_cd_2_spinner->get_float_val(), m_cv_2_spinner->get_float_val());
}

void RagDollApplication::DisableStateSpinner() {

	switch (m_currentState)
	{
	case 0: {
				// Deactivate
				m_url_state_spinner->disable();
				m_lrl_state_spinner->disable();
				m_rf_state_spinner->disable();
				m_ull_state_spinner->disable();
				m_lll_state_spinner->disable();
				m_lf_state_spinner->disable();
				m_torso_state_spinner->disable();
	}
		break;
	case 1:
	case 2: {
				m_url_state_spinner->enable();
				m_lrl_state_spinner->enable();
				m_rf_state_spinner->enable();
				m_torso_state_spinner->enable();
				m_ull_state_spinner->enable();
				m_lll_state_spinner->enable();
				m_lf_state_spinner->enable();
				break;
	}
	case 3:
	case 4:{
			   m_url_state_spinner->enable();
			   m_lrl_state_spinner->enable();
			   m_rf_state_spinner->enable();
			   m_torso_state_spinner->enable();
			   m_ull_state_spinner->enable();
			   m_lll_state_spinner->enable();
			   m_lf_state_spinner->enable();
	}
		break;
	default:
		break;
	}

}

void RagDollApplication::DisableAllSpinners() {

	std::vector<GLUI_Spinner*>spinners = {
		m_torso_kp_spinner, m_torso_kd_spinner,
		m_ull_kp_spinner, m_ull_kd_spinner,
		m_url_kp_spinner, m_url_kd_spinner,
		m_lll_kp_spinner, m_lll_kd_spinner,
		m_lrl_kp_spinner, m_lrl_kd_spinner,
		m_lf_kp_spinner, m_lf_kd_spinner,
		m_rf_kp_spinner, m_rf_kd_spinner,
		m_torso_state_spinner, m_ull_state_spinner,
		m_url_state_spinner, m_lll_state_spinner,
		m_lrl_state_spinner, m_lf_state_spinner,
		m_rf_state_spinner
	};

	for (std::vector<GLUI_Spinner*>::iterator it = spinners.begin(); it != spinners.end(); it++) {
		(*it)->disable();
	}

	m_StatesRadioGroup->disable();

}

void RagDollApplication::EnableGainSpinners() {

	std::vector<GLUI_Spinner*>spinners = {
		m_torso_kp_spinner, m_torso_kd_spinner,
		m_ull_kp_spinner, m_ull_kd_spinner,
		m_url_kp_spinner, m_url_kd_spinner,
		m_lll_kp_spinner, m_lll_kd_spinner,
		m_lrl_kp_spinner, m_lrl_kd_spinner,
		m_lf_kp_spinner, m_lf_kd_spinner,
		m_rf_kp_spinner, m_rf_kd_spinner
	};

	for (std::vector<GLUI_Spinner*>::iterator it = spinners.begin(); it != spinners.end(); it++) {
		(*it)->enable();
	}

}

void RagDollApplication::SaveStates() {
	ChangeState(-1);
	// Save States into file
	m_WalkingController->SaveStates(m_gaits.at(m_currentGait));
}

void RagDollApplication::SaveGains(){
	// Save gains into file
	m_WalkingController->SaveGains(m_gaits.at(m_currentGait));
}

void RagDollApplication::SaveFeedback() {
	m_WalkingController->SaveFeedback(m_gaits.at(m_currentGait));
}

void RagDollApplication::SaveTime() {
	m_WalkingController->SaveTime(m_gaits.at(m_currentGait));
}

void RagDollApplication::Reset() {

	Debug("Reset button pressed");

	m_WalkingController->Reset();

	// Clear Everything
	//GameObject::ClearForces(bodies);
	GameObject::ClearVelocities(m_bodies);

	m_StatesRadioGroup->enable();

	GameObject::DisableObjects(m_bodies);

	m_StatesRadioGroup->set_int_val(0); // State 0
	m_currentState = 0;
	ChangeState(0);
	EnableGainSpinners();
	//m_cameraManager->Reset();
}

void RagDollApplication::Start() {

	DisableAllSpinners();

	Debug("Start button Pressed\n INITIATE WALKING!!!");

	GameObject::EnableObjects(m_bodies);

	m_WalkingController->InitiateWalking();
}

void RagDollApplication::Pause() {

	Debug("Pause button pressed");

	GameObject::DisableObjects(m_bodies);

	m_WalkingController->PauseWalking();
}

void RagDollApplication::ChangeState(int id) {

	//printf("Gait is %s \n", m_gaits.at(m_currentGait).c_str());

	Debug("Previous state" << m_previousState);

	State *previousState = GetState(m_previousState);

	// Update previous state
	previousState->m_torsoAngle = m_torso_state_spinner->get_float_val();
	previousState->m_upperLeftLegAngle = m_ull_state_spinner->get_float_val();
	previousState->m_upperRightLegAngle = m_url_state_spinner->get_float_val();
	previousState->m_lowerLeftLegAngle = m_lll_state_spinner->get_float_val();
	previousState->m_lowerRightLegAngle = m_lrl_state_spinner->get_float_val();
	previousState->m_leftFootAngle = m_lf_state_spinner->get_float_val();
	previousState->m_rightFootAngle = m_rf_state_spinner->get_float_val();

	// Change previous to current state
	DisplayState(m_currentState);
	m_previousState = m_currentState;
	Debug("next state = " << m_currentState);

	switch (m_currentState)
	{
	case 0:
		m_WalkingController->m_ragDollState = STATE_0;
		break;
	case 1:
		m_WalkingController->m_ragDollState = STATE_1;
		break;
	case 2:
		m_WalkingController->m_ragDollState = STATE_2;
		break;
	case 3:
		m_WalkingController->m_ragDollState = STATE_3;
		break;
	case 4:
		m_WalkingController->m_ragDollState = STATE_4;	
		break;
	default:
		break;
	}

	DisableStateSpinner();

	UpdateRagDoll();

}

void RagDollApplication::ChangeGait() {

	printf("Previous gate: %s, Current Gait: %s \n", m_gaits.at(m_previousGait).c_str(), m_gaits.at(m_currentGait).c_str());

	m_previousGait = m_currentGait;
	m_WalkingController->ChangeGait(m_gaits.at(m_currentGait));
	SetupGUIConfiguration();
	DisplayState(m_currentState);

	if (m_WalkingController->m_currentState == RESET) {
		UpdateRagDoll();
	}

}

#pragma endregion GUI

#pragma endregion INITIALIZATION

#pragma region RAG_DOLL

void RagDollApplication::CreateRagDoll(const btVector3 &position) {

	// Create a torso centered at the position
	btVector3 torsoHalfSize(torso_width/2, torso_height/2, 0.0);
	btVector3 ulHalfSize = btVector3(upper_leg_width / 2, upper_leg_height / 2, 0.0f);
	btVector3 llHalfSize = btVector3(lower_leg_width / 2, lower_leg_height / 2, 0.0f);
	btVector3 fHalfSize = btVector3(foot_width / 2, foot_height / 2, 0.0f);
	
	// Create RIGHT LEG	
	m_upperRightLeg = Create2DBox(ulHalfSize, upper_leg_mass, btVector3(0 / 256.0, 153 / 256.0, 0 / 256.0), position + btVector3(0, 0, 0)); // Green
	m_rightFoot = Create2DBox(fHalfSize, feet_mass, btVector3(153 / 256.0, 0 / 256.0, 153 / 256.0), position + btVector3(0, 0, 0)); // purple
	m_lowerRightLeg = Create2DBox(llHalfSize, lower_leg_mass, btVector3(255 / 256.0, 102 / 256.0, 0 / 256.0), position + btVector3(0, 0, 0)); // Orange

	m_torso = Create2DBox(torsoHalfSize, torso_mass, btVector3(0, 51 / 256.0, 102 / 256.0), position); // Blue

	// Create LEFT LEG
	m_upperLeftLeg = Create2DBox(ulHalfSize, upper_leg_mass, btVector3(255 / 256.0, 102 / 256.0, 255 / 256.0), position + btVector3(0, 0, 0)); // Pink
	m_leftFoot = Create2DBox(fHalfSize, feet_mass, btVector3(0 / 256.0, 255 / 256.0, 255 / 256.0), position + btVector3(0, 0, 0)); // aqua blue
	m_lowerLeftLeg = Create2DBox(llHalfSize, lower_leg_mass, btVector3(250 / 256.0, 250 / 256.0, 10 / 256.0), position + btVector3(0, 0, 0)); // Yellow
	
	AddHinges();

	m_bodies = { m_torso, m_upperLeftLeg, m_upperRightLeg, m_lowerLeftLeg, m_lowerRightLeg, m_leftFoot, m_rightFoot, };

}

void RagDollApplication::AddHinges() {

	Debug("Adding hinges.");

	// Connect torso to upper legs
	m_torso_ulLeg = AddHingeConstraint(m_torso, m_upperLeftLeg, btVector3(0, -torso_height / 2, 0), btVector3(0, upper_leg_height / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_HIGH));
	m_torso_urLeg = AddHingeConstraint(m_torso, m_upperRightLeg, btVector3(0, -torso_height / 2, 0), btVector3(0, upper_leg_height / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_URL_HIGH));

	// Connect upper legs to lower legs
	m_ulLeg_llLeg = AddHingeConstraint(m_upperLeftLeg, m_lowerLeftLeg, btVector3(0, -upper_leg_height / 2, 0), btVector3(0, lower_leg_height / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_ULL_LLL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_ULL_LLL_HIGH));
	m_urLeg_lrLeg = AddHingeConstraint(m_upperRightLeg, m_lowerRightLeg, btVector3(0, -upper_leg_height / 2, 0), btVector3(0, lower_leg_height / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_URL_LRL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_URL_LRL_HIGH));

	// Connect feet to lower legs
	m_llLeg_lFoot = AddHingeConstraint(m_lowerLeftLeg, m_leftFoot, btVector3(0, -lower_leg_height / 2, 0), btVector3(-(foot_width) / 4, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_LLL_LF_LOW), Constants::GetInstance().DegreesToRadians(HINGE_LLL_LF_HIGH));
	m_lrLeg_rFoot = AddHingeConstraint(m_lowerRightLeg, m_rightFoot, btVector3(0, -lower_leg_height / 2, 0), btVector3(-(foot_width) / 4, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_LRL_RF_LOW), Constants::GetInstance().DegreesToRadians(HINGE_LRL_RF_HIGH));

}

void RagDollApplication::UpdateRagDoll() {

	State *state = GetState(m_currentState);

	float torsoAngle = Constants::GetInstance().DegreesToRadians(state->m_torsoAngle);
	float ullAngle = Constants::GetInstance().DegreesToRadians(state->m_upperLeftLegAngle);
	float urlAngle = Constants::GetInstance().DegreesToRadians(state->m_upperRightLegAngle);
	// Local coordinates convert to world coordinates
	float lllAngle = Constants::GetInstance().DegreesToRadians(state->m_upperLeftLegAngle - state->m_lowerLeftLegAngle);
	float lrlAngle = Constants::GetInstance().DegreesToRadians(state->m_upperRightLegAngle - state->m_lowerRightLegAngle);

	//printf("lll angle = %f, lfAngle = %f\n", Constants::GetInstance().RadiansToDegrees(lllAngle), Constants::GetInstance().RadiansToDegrees(lllAngle) - state->m_leftFootAngle - 90);
	//printf("state lfAngle: %f \n", state->m_leftFootAngle);
	float lfAngle = lllAngle - Constants::GetInstance().DegreesToRadians(state->m_leftFootAngle);
	float rfAngle = lrlAngle - Constants::GetInstance().DegreesToRadians(state->m_rightFootAngle);

	//printf("Updating Rag doll:\n %f, %f, %f, %f, %f, %f, %f\n", torsoAngle, ullAngle, urlAngle, lllAngle, lrlAngle, lfAngle, rfAngle);
	// Blue
	m_torso->Reposition(
		ORIGINAL_TORSO_POSITION + btVector3(-(torso_height / 2) * sin(torsoAngle), (torso_height / 2) * (cos(torsoAngle) - 1), 0),
		btQuaternion(btVector3(0, 0, 1), torsoAngle));
	Debug("Torso COM (" << m_torso->GetCOMPosition().x() << ", " << m_torso->GetCOMPosition().y() << ", " << m_torso->GetCOMPosition().z() << ")");

	//("torso angle = %f, orientation = %f \n", Constants::GetInstance().RadiansToDegrees(torsoAngle), m_torso->GetOrientation());

	btVector3 hipPosition = ORIGINAL_TORSO_POSITION + btVector3(0, -torso_height/2, 0);  // Hip stays constant

	// GREEN
	m_upperRightLeg->Reposition(
		hipPosition + btVector3(0, - upper_leg_height/2, 0.1) 
		+ btVector3((upper_leg_height/2) * sin(urlAngle), (upper_leg_height/2)*(1-cos(urlAngle)), 0),
		btQuaternion(btVector3(0, 0, 1), urlAngle));
	Debug("URL COM (" << m_upperRightLeg->GetCOMPosition().x() << ", " << m_upperRightLeg->GetCOMPosition().y() << ", " << m_upperRightLeg->GetCOMPosition().z(), ")");

	//printf("URL angle = %f, orientation = %f \n", Constants::GetInstance().RadiansToDegrees(urlAngle), m_upperRightLeg->GetOrientation());

	// PINK
	m_upperLeftLeg->Reposition(
		hipPosition + btVector3(0, -upper_leg_height / 2, -0.1)
		+ btVector3((upper_leg_height / 2) * sin(ullAngle), (upper_leg_height / 2)*(1 - cos(ullAngle)), 0),
		btQuaternion(btVector3(0, 0, 1), ullAngle));
	Debug("ULL COM (" << m_upperLeftLeg->GetCOMPosition().x() << ", " << m_upperLeftLeg->GetCOMPosition().y() << ", " << m_upperLeftLeg->GetCOMPosition().z() << ")");

	btVector3 upperRightLegBottomPoint = m_upperRightLeg->GetCOMPosition() + btVector3(sin(urlAngle) * upper_leg_height / 2, -cos(urlAngle) * upper_leg_height / 2, 0);

	// ORANGE
	m_lowerRightLeg->Reposition(
		upperRightLegBottomPoint + btVector3((lower_leg_height / 2) * sin(lrlAngle), -(lower_leg_height/2)*(cos(lrlAngle)), 0.1),
		btQuaternion(btVector3(0, 0, 1), lrlAngle));
	Debug("LRL COM (" << m_lowerRightLeg->GetCOMPosition().x() << ", " << m_lowerRightLeg->GetCOMPosition().y() << ", " << m_lowerRightLeg->GetCOMPosition().z(), ")");

	//printf("lower right leg angle = %f, orientation = %f\n", Constants::GetInstance().RadiansToDegrees(lrlAngle), m_lowerRightLeg->GetOrientation());

	btVector3 upperLeftLegBottomPoint = m_upperLeftLeg->GetCOMPosition() + btVector3(sin(ullAngle) * upper_leg_height / 2, -cos(ullAngle) * upper_leg_height / 2, 0);
	
	m_lowerLeftLeg->Reposition(
		upperLeftLegBottomPoint + btVector3((lower_leg_height / 2) * sin(lllAngle), -(lower_leg_height / 2) * cos(lllAngle), -0.1),
		btQuaternion(btVector3(0, 0, 1), lllAngle));
	
	Debug("LLL COM (" << m_lowerLeftLeg->GetCOMPosition().x() << ", " << m_lowerLeftLeg->GetCOMPosition().y() << ", " << m_lowerLeftLeg->GetCOMPosition().z() << ")");

	btVector3 lowerRightLegBottomPoint = m_lowerRightLeg->GetCOMPosition() + btVector3(sin(lrlAngle) * lower_leg_height / 2, -cos(lrlAngle) * lower_leg_height / 2, 0);
	// PURPLE
	m_rightFoot->Reposition(
		lowerRightLegBottomPoint + btVector3(cos(rfAngle) * foot_width / 4, sin(rfAngle) * foot_width / 4, 0.1),
		btQuaternion(btVector3(0, 0, 1), rfAngle));
	Debug("RF COM (" << m_rightFoot->GetCOMPosition().x() << ", " << m_rightFoot->GetCOMPosition().y() << ", " << m_rightFoot->GetCOMPosition().z() << ")");
	
	//printf("right foot angle = %f, orientation = %f\n", Constants::GetInstance().RadiansToDegrees(rfAngle), m_rightFoot->GetOrientation());

	//printf("ULL BP position: %f, %f\n", upperLeftLegBottomPoint.x(), upperLeftLegBottomPoint.y());
	// Yellow
	//m_lowerLeftLeg->Reposition(upperLeftLegBottomPoint + btVector3(0.0f, -lower_leg_height / 2, 0.1), btQuaternion(btVector3(0, 0, 1), lllAngle));
	
	
	btVector3 lowerLeftLegBottomPoint = m_lowerLeftLeg->GetCOMPosition() + btVector3(sin(lllAngle) * lower_leg_height / 2, -cos(lllAngle) * lower_leg_height / 2, 0);

	//printf("Before: lf orientation = %f, lfAngle = %f\n", m_leftFoot->GetOrientation(), Constants::GetInstance().RadiansToDegrees(lfAngle));

	m_leftFoot->Reposition(
		lowerLeftLegBottomPoint + btVector3(cos(lfAngle) * foot_width / 4, sin(lfAngle) * foot_width / 4, -0.1),
		btQuaternion(btVector3(0, 0, 1), lfAngle));
	
	//printf("After: lf orientation = %f, lfAngle = %f\n", m_leftFoot->GetOrientation(), Constants::GetInstance().RadiansToDegrees(lfAngle));
	Debug("LF COM (" << m_leftFoot->GetCOMPosition().x() << ", " << m_leftFoot->GetCOMPosition().y() << ", " << m_leftFoot->GetCOMPosition().z() << ")");

	//GameObject::PrintOrientations(m_bodies);

}

void RagDollApplication::ChangeTorsoAngle() {
	State *state = GetState(m_currentState);
	state->m_torsoAngle = m_torso_state_spinner->get_float_val();
	UpdateRagDoll();
}

void RagDollApplication::ChangeUpperLeftLegAngle() {
	State *state = GetState(m_currentState);
	state->m_upperLeftLegAngle = m_ull_state_spinner->get_float_val();
	UpdateRagDoll();
}

void RagDollApplication::ChangeUpperRightLegAngle() {
	State *state = GetState(m_currentState);
	state->m_upperRightLegAngle = m_url_state_spinner->get_float_val();
	UpdateRagDoll();
}

void RagDollApplication::ChangeLowerLeftLegAngle() {
	State *state = GetState(m_currentState);
	state->m_lowerLeftLegAngle = m_lll_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

void RagDollApplication::ChangeLowerRightLegAngle() {
	State *state = GetState(m_currentState);
	state->m_lowerRightLegAngle = m_lrl_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

void RagDollApplication::ChangeLeftFootAngle() {
	State *state = GetState(m_currentState);
	state->m_leftFootAngle = m_lf_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

void RagDollApplication::ChangeRightFootAngle() {
	State *state = GetState(m_currentState);
	state->m_rightFootAngle = m_rf_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

State *RagDollApplication::GetState(int state) {
	switch (state)
	{
	case 0:
		return m_WalkingController->m_state0;
		break;
	case 1:
		return m_WalkingController->m_state1;
		break;
	case 2:
		return m_WalkingController->m_state2;
		break;
	case 3:
		return m_WalkingController->m_state3;
		break;
	case 4:
		return m_WalkingController->m_state4;
		break;
	default:
		break;
	}
}


#pragma region TORQUES

// Upper legs
void RagDollApplication::ApplyTorqueOnUpperLeftLeg(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_upperLeftLeg, torque);
}

void RagDollApplication::ApplyTorqueOnUpperRightLeg(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_upperRightLeg, torque);
}

// Lower legs
void RagDollApplication::ApplyTorqueOnLowerLeftLeg(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_lowerLeftLeg, torque);
}

void RagDollApplication::ApplyTorqueOnLowerRightLeg(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_lowerRightLeg, torque);
}

// Feet
void RagDollApplication::ApplyTorqueOnLeftFoot(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_leftFoot, torque);
}

void RagDollApplication::ApplyTorqueOnRightFoot(float torqueForce) {
	btVector3 torque(btVector3(0, 0, torqueForce));
	ApplyTorque(m_rightFoot, torque);
}

#pragma endregion TORQUES

#pragma endregion RAG_DOLL

#pragma region DRAWING

GameObject *RagDollApplication::Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {

	// Multiply size by scaling factor
	GameObject *aBox = CreateGameObject(new btBox2dShape(halfSize), mass, color, position);

	return aBox;
}

GameObject *RagDollApplication::Create2DCircle(float radius, float mass, const btVector3 &color, const btVector3 &position) {
	GameObject *aCircle = CreateGameObject(new btSphereShape(radius), mass, color, position);
	return aCircle;
}

GameObject *RagDollApplication::Create2DLine(const btVector3 &start, const btVector3 &end, const btVector3 &color) {
	float distance = start.distance(end);
	btVector3 halfsize(0.1, distance, 0);
	btVector3 midpoint((start.x() + end.x()) / 2, (start.y() + end.y()) / 2, 0);
	return Create2DBox(halfsize, 0, btVector3(1.0f, 0.0f, 0.0f), midpoint);
}

GameObject *RagDollApplication::Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {

	// Multiply size by scaling factor
	GameObject *aBox = CreateGameObject(new btBoxShape(halfSize), mass, color, position);

	return aBox;
}

btVector3 RagDollApplication::GetRandomColor() {
	return btVector3(((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX));
}

void RagDollApplication::DrawDebugFeedback() {

	//DrawFilledCircle(0, -1 + GROUND_HEIGHT/2, 0.05, btVector3(255, 0, 0));

	if (m_drawBackForce && !m_drawForwardForce) {
		DrawArrow(m_torso->GetCOMPosition(), LEFT);
	}
	if (m_drawForwardForce && !m_drawBackForce) {
		DrawArrow(m_torso->GetCOMPosition(), RIGHT);
	}

	// Track the torso with the camera
	btVector3 newTorsoPosition = m_WalkingController->m_COMPosition;
	//printf(">>>new torso position x = %f \n", newTorsoPosition.x());
	float to_Translate = newTorsoPosition.x() - m_previous_torso_position.x();
	m_cameraManager->TranslateCamera(RIGHT, to_Translate);
	m_previous_torso_position = newTorsoPosition;

	// Get Stance ankle location
	btVector3 stanceAnkle = m_WalkingController->m_stanceAnklePosition;
	// Draw circle
	DrawFilledCircle(stanceAnkle.x(), stanceAnkle.y(), 0.03, btVector3(255, 255, 255));
	// Get COM Position
	btVector3 COMPosition = m_WalkingController->m_COMPosition;
	DrawFilledCircle(COMPosition.x(), COMPosition.y(), 0.03, btVector3(255, 255, 255));
	// Draw horizontal line between ankle and COM
	
}

void RagDollApplication::DrawArrow(const btVector3 &pointOfContact, TranslateDirection direction) {
	
	glColor3f(255, 255, 255);
	glPushMatrix();

	static int indices[9] = {
		0, 1, 2,
		3, 4, 5,
		4, 5, 6
	};
	switch (direction)
	{
	case UP:
		break;
	case DOWN:
		break;
	case LEFT: {
		btVector3 vertices[7] = {
			btVector3(pointOfContact.x(), pointOfContact.y(), 0.0f),
			btVector3(pointOfContact.x() + 0.1, pointOfContact.y() + 0.15, 1.0f),
			btVector3(pointOfContact.x() + 0.1, pointOfContact.y() - 0.15, 1.0f),
			btVector3(pointOfContact.x() + 0.1, pointOfContact.y() + 0.05, 1.0f),
			btVector3(pointOfContact.x() + 0.1, pointOfContact.y() - 0.05, 1.0f),
			btVector3(pointOfContact.x() + 0.35, pointOfContact.y() + 0.05, 1.0f),
			btVector3(pointOfContact.x() + 0.35, pointOfContact.y() - 0.05, 1.0f),
		};
		DrawWithTriangles(vertices, indices, 9);
	}

		break;
	case RIGHT: {
		btVector3 vertices[7] = {
			btVector3(pointOfContact.x(), pointOfContact.y(), 0.0f),
			btVector3(pointOfContact.x() - 0.1, pointOfContact.y() + 0.15, 1.0f),
			btVector3(pointOfContact.x() - 0.1, pointOfContact.y() - 0.15, 1.0f),
			btVector3(pointOfContact.x() - 0.1, pointOfContact.y() + 0.05, 1.0f),
			btVector3(pointOfContact.x() - 0.1, pointOfContact.y() - 0.05, 1.0f),
			btVector3(pointOfContact.x() - 0.35, pointOfContact.y() + 0.05, 1.0f),
			btVector3(pointOfContact.x() - 0.35, pointOfContact.y() - 0.05, 1.0f),
		};
		DrawWithTriangles(vertices, indices, 9);
	}
		break;
	default:
		break;
	}
}

void RagDollApplication::DrawShape(btScalar *transform, const btCollisionShape *pShape, const btVector3 &color) {
	
	//BulletOpenGLApplication::DrawShape(transform, pShape, color);
	
	// Special rendering
	if (pShape->getUserPointer() == m_torso) {
		const btBoxShape *box = static_cast<const btBoxShape*>(pShape);
		btVector3 halfSize = box->getHalfExtentsWithMargin();

		glColor3f(color.x(), color.y(), color.z());

		// push the matrix stack

		glPushMatrix();
		glMultMatrixf(transform);

		DrawTorso(halfSize);

		glPopMatrix();
	}
	else if (pShape->getUserPointer() == m_upperLeftLeg || pShape->getUserPointer() == m_upperRightLeg) {
		// Draw Upper legs
		const btBoxShape *box = static_cast<const btBoxShape*>(pShape);
		btVector3 halfSize = box->getHalfExtentsWithMargin();
		glColor3f(color.x(), color.y(), color.z());
		glPushMatrix();
		glMultMatrixf(transform);

		DrawUpperLeg(halfSize);

		glPopMatrix();

	}
	else if (pShape->getUserPointer() == m_lowerLeftLeg || pShape->getUserPointer() == m_lowerRightLeg) {
		// Draw Lower legs
		const btBoxShape *box = static_cast<const btBoxShape*>(pShape);
		btVector3 halfSize = box->getHalfExtentsWithMargin();
		glColor3f(color.x(), color.y(), color.z());
		glPushMatrix();
		glMultMatrixf(transform);

		DrawLowerLeg(halfSize);

		glPopMatrix();

	}
	else if (pShape->getUserPointer() == m_leftFoot || pShape->getUserPointer() == m_rightFoot) {
		// Draw feet
		const btBoxShape *box = static_cast<const btBoxShape*>(pShape);
		btVector3 halfSize = box->getHalfExtentsWithMargin();

		glColor3f(color.x(), color.y(), color.z());
		glPushMatrix();
		glMultMatrixf(transform);

		DrawFoot(halfSize);

		glPopMatrix();

	}
	else {
		BulletOpenGLApplication::DrawShape(transform, pShape, color);
	}
	
}

void RagDollApplication::DrawTorso(const btVector3 &halfSize) {

	float halfHeight = halfSize.y();
	float halfWidth = halfSize.x();
	float halfDepth = halfSize.z(); // No depth

	float shoulderRadius = 1.5 * halfWidth;
	float hipRadius = 1.25 * halfWidth;

	// Create Vector
	btVector3 vertices[4] = {
		btVector3(-shoulderRadius, halfHeight, 0.0f),	// 0
		btVector3(shoulderRadius, halfHeight, 0.0f),	// 1
		btVector3(-hipRadius, -halfHeight, 0.0f),		// 2 
		btVector3(hipRadius, -halfHeight, 0.0f),		// 3

	};

	static int indices[6] = {
		0, 1, 2,
		3, 2, 1
	};

	DrawWithTriangles(vertices, indices, 6);

	// Create semisircle for shoulders
	DrawPartialFilledCircle(0, halfHeight,  shoulderRadius, 0, 180);
	DrawPartialFilledCircle(0, -2*halfHeight, hipRadius, 180, 360);

}

void RagDollApplication::DrawUpperLeg(const btVector3 &halfSize) {

	float halfHeight = halfSize.y();
	float halfWidth = halfSize.x();
	float halfDepth = halfSize.z(); // No depth

	float thighRadius = 1.25f * halfWidth;
	float kneeRadius = .9f * halfWidth;

	// Create Vector
	btVector3 vertices[4] = {
		btVector3(- thighRadius, halfHeight, 0.0f),		// 0
		btVector3(thighRadius, halfHeight, 0.0f),		// 1
		btVector3(-kneeRadius, -halfHeight, 0.0f),		// 2 
		btVector3(kneeRadius, -halfHeight, 0.0f),		// 3

	};

	static int indices[6] = {
		0, 1, 2,
		3, 2, 1
	};

	DrawWithTriangles(vertices, indices, 6);

	// Create semisircle for thigh and knees
	DrawPartialFilledCircle(0, halfHeight, thighRadius, 0, 180);
	DrawPartialFilledCircle(0, -2 * halfHeight, kneeRadius, 180, 360);

}

void RagDollApplication::DrawLowerLeg(const btVector3 &halfSize){

	float halfHeight = halfSize.y();
	float halfWidth = halfSize.x();
	float halfDepth = halfSize.z(); // No depth

	float kneeRadius = 1.25f * halfWidth;
	float ankleRadius = halfWidth;

	// Create Vector
	btVector3 vertices[4] = {
		btVector3(-kneeRadius, halfHeight, 0.0f),		// 0
		btVector3(kneeRadius, halfHeight, 0.0f),		// 1
		btVector3(-ankleRadius, -halfHeight, 0.0f),	// 2 
		btVector3(ankleRadius, -halfHeight, 0.0f),		// 3

	};

	static int indices[6] = {
		0, 1, 2,
		3, 2, 1
	};

	DrawWithTriangles(vertices, indices, 6);

	// Create semisircle for thigh and knees
	DrawPartialFilledCircle(0, halfHeight, kneeRadius, 0, 180);
	//DrawPartialFilledCircle(-2*halfHeight, 0, ankleRadius, 90, 270);

}

void RagDollApplication::DrawFoot(const btVector3 &halfSize) {

	float halfWidth = halfSize.x();
	float halfHeight = halfSize.y();
	float halfDepth = halfSize.z(); // No depth

	float toeRadius = halfHeight * 2;

	// Create Vector
	btVector3 vertices[4] = {
		btVector3(-halfWidth, halfHeight, 0.0f),		// 0
		btVector3(-halfWidth, -halfHeight, 0.0f),	// 1
		btVector3((halfWidth - 2 * halfHeight), -halfHeight, 0.0f),	// 2 
		btVector3((halfWidth - 2 * halfHeight), halfHeight, 0.0f),	// 3

	};

	static int indices[6] = {
		0, 1, 2,
		3, 2, 0
	};

	DrawWithTriangles(vertices, indices, 6);

	// Create quarter circle for toes
	//DrawPartialFilledCircle(, , toeRadius, 0, 180);
	DrawPartialFilledCircle((halfWidth - 2 * halfHeight), -halfHeight, toeRadius, 0, 90);

}

static void DrawPartialFilledCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat begin, GLfloat end) {
	/* Draw CCW from begin to end */
	int triangleAmount = 20; //# of triangles used to draw circle

	glTranslatef(x, y, 0);

	begin = Constants::GetInstance().DegreesToRadians(begin);
	end = Constants::GetInstance().DegreesToRadians(end);
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0, 0);
	for (int i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			(radius * cos(i *  (end - begin) / triangleAmount + begin)),
			(radius * sin(i * (end - begin) / triangleAmount + begin))
			);
	}
	glEnd();

}

static void DrawFilledCircle(GLfloat x, GLfloat y, GLfloat radius, const btVector3 &color){
	int triangleAmount = 20; //# of triangles used to draw circle
	glColor3f(color.x(), color.y(), color.z());
	glPushMatrix();
	glTranslatef(x, y, 0.9);
	//GLfloat radius = 0.8f; //radius
	
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			(radius * cos(i *  TWO_PI / triangleAmount)),
			(radius * sin(i * TWO_PI / triangleAmount))
			);
	}
	glEnd();

	glPopMatrix();
}

static void DrawLine(const btVector3 &begin, const btVector3 &end, const btVector3 &color) {

	glColor3f(color.x(), color.y(), color.z());
	glLineWidth(2.5);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(begin.x(), begin.y(), begin.z());
	glVertex3f(end.x(), end.y(), end.z());
	glEnd();
}

#pragma endregion DRAWING

#pragma region GLUI_CALLBACKS

static void RagDollIdle() {
	m_app->Idle();
}

static void SaveGainsButtonPressed(int id) {
	m_app->SaveGains();
}

static void SaveStatesButtonPressed(int id) {
	m_app->SaveStates();
}

static void SaveFeedbackButtonPressed(int id) {
	m_app->SaveFeedback();
}

static void SaveTimeButtonPressed(int id) {
	m_app->SaveTime();
}

static void ResetButtonPressed(int id) {
	m_app->Reset();
}

static void StartButtonPressed(int id) {
	m_app->Start();
}

static void PauseButtonPressed(int id) {
	m_app->Pause();
}

static void StateChanged(int id)	 {
	m_app->ChangeState(id);
}

static void GaitsChanged(int id) {
	m_app->ChangeGait();
}

// State Callbacks

static void TorsoAngleChanged(int id) {
	m_app->ChangeTorsoAngle();
}

static void UpperLeftLegAngleChanged(int id) {
	m_app->ChangeUpperLeftLegAngle();
}

static void UpperRightLegAngleChanged(int id) {
	m_app->ChangeUpperRightLegAngle();
}

static void LowerLeftLegAngleChanged(int id) {
	m_app->ChangeLowerLeftLegAngle();
}

static void LowerRightLegAngleChanged(int id) {
	m_app->ChangeLowerRightLegAngle();
}

static void LeftFootAngleChanged(int id) {
	m_app->ChangeLeftFootAngle();
}

static void RightFootAngleChanged(int id) {
	m_app->ChangeRightFootAngle();
}

static void GainsChanged(int id) {
	m_app->UpdateGains();
}

static void FeedbackChanged(int id) {
	m_app->UpdateFeedbacks();
}

#pragma endregion GLUI_CALLBACKS

void InternalPreTickCallback(btDynamicsWorld *world, btScalar timeStep)  {
	m_app->RagDollStep();
}

void InternalPostTickCallback(btDynamicsWorld *world, btScalar timestep) {
	m_app->RagDollCollision();
}