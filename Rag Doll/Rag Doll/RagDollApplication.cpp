#include "stdafx.h"
#include "RagDollApplication.h"
#include "WalkingController.h"

#pragma region INITIALIZATION

static RagDollApplication *m_app;

enum ControlIDs { 
	TORSO_KP, TORSO_KD, 
	UPPER_R_LEG_KP, UPPER_R_LEG_KD, 
	UPPER_L_LEG_KP, UPPER_L_LEG_KD, 
	LOWER_R_LEG_KP, LOWER_R_LEG_KD, 
	LOWER_L_LEG_KP, LOWER_L_LEG_KD, 
	R_FOOT_KP, R_FOOT_KD, 
	L_FOOT_KP, L_FOOT_KD };

#define KP_LOWER 0.0
#define KP_HIGHER 5.0
#define KD_LOWER 0.0
#define KD_HIGHER 5.0

RagDollApplication::RagDollApplication()
{
}

RagDollApplication::RagDollApplication(ProjectionMode mode) :BulletOpenGLApplication(mode){
	m_app = this;
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

	// Create ground
	btVector3 ground(1.0f, 25.0f, 10.0f);
	float mass = 0.0f;
	btVector3 position(0.0f, -10.0f, 0.0f);
	Create3DBox(ground, mass, GetRandomColor(), position);

	CreateRagDoll(btVector3(0, 3, 0.5));

	// Create Controller
	m_WalkingController = new WalkingController(this);

	// Create GUI
	CreateRagDollGUI();
}

void RagDollApplication::CreateRagDollGUI() {

	printf("Create Glui Window \n");

	// Setup
	GLUI_Master.set_glutIdleFunc(RagDollIdle);
	m_glui_window = GLUI_Master.create_glui("Rag Doll Controls", 0, 1);
	m_glui_window->set_main_gfx_window(m_main_window_id);

	// Controls
	// Gains Panel
	GLUI_Panel *gains_panel = m_glui_window->add_panel("Gains");
	m_glui_window->add_statictext_to_panel(gains_panel, "Torso");
	GLUI_Spinner *torso_kp_spinner =  m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, TORSO_KP);
	GLUI_Spinner *torso_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, TORSO_KP);

	torso_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	torso_kd_spinner->set_float_limits(KP_LOWER, KP_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Left Leg");
	GLUI_Spinner *ul_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, UPPER_L_LEG_KP);
	GLUI_Spinner *ul_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, UPPER_L_LEG_KD);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Right Leg");
	GLUI_Spinner *ur_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KP);
	GLUI_Spinner *ur_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KD);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Left Leg");
	GLUI_Spinner *ll_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, LOWER_L_LEG_KP);
	GLUI_Spinner *ll_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, LOWER_L_LEG_KD);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Right Leg");
	GLUI_Spinner *lr_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, LOWER_R_LEG_KP);
	GLUI_Spinner *lr_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, LOWER_R_LEG_KD);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Left Foot");
	GLUI_Spinner *lf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, L_FOOT_KP);
	GLUI_Spinner *lf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, L_FOOT_KD);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Right foot");
	GLUI_Spinner *uf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KP);
	GLUI_Spinner *uf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, NULL, UPPER_R_LEG_KD);

	// Horizontal separation
	m_glui_window->add_column(true);

	// States
	GLUI_Panel *states_panel = m_glui_window->add_panel("States");
	GLUI_RadioGroup *states_radio_group = m_glui_window->add_radiogroup_to_panel(states_panel, NULL, -1);

	m_glui_window->add_radiobutton_to_group(states_radio_group, "State 1");
	m_glui_window->add_radiobutton_to_group(states_radio_group, "State 2");
	m_glui_window->add_radiobutton_to_group(states_radio_group, "State 3");
	m_glui_window->add_radiobutton_to_group(states_radio_group, "State 4");



}

void RagDollApplication::ShutdownPhysics() {

}

#pragma endregion INITIALIZATION

#pragma region RAG_DOLL

void RagDollApplication::CreateRagDoll(const btVector3 &position) {

	// MASS
	float torso_mass = 70;
	float upper_leg_mass = 5;
	float lower_leg_mass = 4;
	float feet_mass = 1;

	// DIMENSIONS
	float torso_width = 1;
	float torso_height = 2.5;

	float upper_leg_height = 3.0f;
	float upper_leg_width = 0.7f;

	float lower_leg_height = 3.0f;
	float lower_leg_width = 0.5f;

	float foot_height = 0.3;
	float foot_width = 0.9;

	// Create a torso centered at the position
	btVector3 halfSize(torso_height/2, torso_width/2, 0.0);
	
	GameObject *torso = Create2DBox(halfSize, torso_mass, GetRandomColor(), position);

	// test box
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(-4, 5, 0.7));
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(4, 7, 0.5));

	// Create Upper legs
	halfSize = btVector3(upper_leg_height / 2, upper_leg_width / 2, 0.0f);
	btVector3 pos1 = position;
	pos1.setZ(pos1.getZ() - 0.25);
	m_upperLeftLeg = Create2DBox(halfSize, upper_leg_mass, GetRandomColor(), pos1);
	btVector3 pos2 = position;
	pos2.setZ(pos2.getZ() + 0.25);
	m_upperRightLeg = Create2DBox(halfSize, upper_leg_mass, GetRandomColor(), pos2);

	// Create lower legs
	halfSize = btVector3(lower_leg_height / 2, lower_leg_width / 2, 0.0f);
	btVector3 pos3 = position;
	pos3.setZ(pos3.getZ() - 0.27);
	m_lowerLeftLeg = Create2DBox(halfSize, lower_leg_mass, GetRandomColor(), pos3);
	btVector3 pos4 = position;
	pos4.setZ(pos4.getZ() + 0.27);
	m_lowerRightLeg = Create2DBox(halfSize, lower_leg_mass, GetRandomColor(), pos4);

	// Create feet
	halfSize = btVector3(foot_height / 2, foot_width / 2, 0.0f);
	btVector3 pos5 = position;
	pos5.setZ(pos5.getZ() - 0.28);
	m_leftFoot = Create2DBox(halfSize, feet_mass, GetRandomColor(), pos5);
	halfSize = btVector3(foot_height / 2, foot_width / 2, 0.0f);
	btVector3 pos6 = position;
	pos6.setZ(pos6.getZ() + 0.28);
	m_rightFoot = Create2DBox(halfSize, feet_mass, GetRandomColor(), pos6);

	// Connect torso to upper legs
	AddHingeConstraint(torso, m_upperLeftLeg, btVector3(-torso_height / 2, 0, 0), btVector3(upper_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(-90.0f), Constants::GetInstance().DegreesToRadians(30.0f));
	AddHingeConstraint(torso, m_upperRightLeg, btVector3(-torso_height / 2, 0, 0), btVector3(upper_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(-90.0f), Constants::GetInstance().DegreesToRadians(30.0f));

	// Connect upper legs to lower legs
	AddHingeConstraint(m_upperLeftLeg, m_lowerLeftLeg, btVector3(-upper_leg_height / 2, 0, 0), btVector3(lower_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(90.0f));
	AddHingeConstraint(m_upperRightLeg, m_lowerRightLeg, btVector3(-upper_leg_height / 2, 0, 0), btVector3(lower_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(90.0f));

	// Connect feet to lower legs
	AddHingeConstraint(m_lowerLeftLeg, m_leftFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3(0, foot_width / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(90.0f));
	AddHingeConstraint(m_lowerRightLeg, m_rightFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3(0, foot_width / 2, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(0.0f), Constants::GetInstance().DegreesToRadians(90.0f));
}

void RagDollApplication::ApplyTorqueOnTorso(float torqueForce) {
	ApplyTorqueOnGameBody(m_torso, torqueForce);
}

void RagDollApplication::ApplyTorqueOnUpperRightLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_upperRightLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnUpperLeftLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_upperLeftLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLowerRightLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_lowerRightLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLowerLeftLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_lowerLeftLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnRightFoot(float torqueForce) {
	ApplyTorqueOnGameBody(m_rightFoot, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLeftFoot(float torqueForce) {
	ApplyTorqueOnGameBody(m_leftFoot, torqueForce);
}

void RagDollApplication::ApplyTorqueOnGameBody(GameObject *body, float torqueForce) {
	// make btVector3
	// apply torque to body
}

#pragma endregion RAG_DOLL

#pragma region DRAWING

GameObject *RagDollApplication::Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {

	GameObject *aBox = CreateGameObject(new btBox2dShape(halfSize), mass, color, position);
	return aBox;
}

GameObject *RagDollApplication::Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {
	GameObject *aBox = CreateGameObject(new btBoxShape(halfSize), mass, color, position);
	return aBox;
}


btVector3 RagDollApplication::GetRandomColor() {
	return btVector3(((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX));
}

#pragma endregion DRAWING

#pragma region GLUI_CALLBACKS

static void RagDollIdle() {

	//m_app->Idle();

	//if (glutGetWindow() != m_app->m_main_window_id) {
	//	glutSetWindow(m_app->m_main_window_id);
	//}

	//glutPostRedisplay();
	m_app->Idle();

}

#pragma endregion GLUI_CALLBACKS