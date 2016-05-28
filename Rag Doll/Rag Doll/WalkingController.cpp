#include "stdafx.h"

#include "WalkingController.h"
#include "RagDollApplication.h"
#include "State.h"
#include "Gains.h"

#include "dirent.h"
#include <fstream>

#include <limits.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

#define DURATION_1 0.3
#define DURATION_2 0.3

WalkingController::WalkingController()
{
}

WalkingController::WalkingController(RagDollApplication *app) {
	m_app = app;
	m_ragDollState = STATE_0;
	m_currentState = RESET;	
}

std::vector<State *> WalkingController::ReadStateFile() {

	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string state_ext = "cfg";
		
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(state_ext, (fname.length() - state_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float torso, ull, url, lll, lrl, lf, rf;
					char c;
					//printf("States: \n");
					int state = 0;
					while ((infile >> torso >> c >> ull >> c >> url >> c >> lll >> c >> lrl >> c >> lf >> c >> rf) && (c == ',')) {
						//printf("%f, %f, %f, %f, %f, %f, %f \n", torso, ull, url, lll, lrl, lf, rf);
						// Set GLUI to read parameters
						switch (state)
						{
						case 0:
							m_state0 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 1:
							m_state1 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 2:
							m_state2 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 3:
							m_state3 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						case 4:
							m_state4 = new State(torso, ull, url, lll, lrl, lf, rf);
							break;
						default:
							break;
						}

						state++;
					}
				}
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		// Initialize state to be zeros
		m_state0 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state1 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state2 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state3 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		m_state4 = new State(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	}

	std::vector<State *> states = {m_state0, m_state1, m_state2, m_state3, m_state4};
	return states;

}

std::vector<Gains *> WalkingController::ReadGainsFile() {

	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string gains_ext = "gns";
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(gains_ext, (fname.length() - gains_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float kp, kd;
					char c;
					int body = 0;
					printf("Gains: \n");
					while ((infile >> kp >> c >> kd) && (c == ',')) {
						//printf("%f, %f \n", kp, kd);
						// Set GLUI to read parameters
						switch (body)
						{
						case 0:
							// Torso
							m_torso_gains = new Gains(kp, kd, TORSO);
							break;
						case 1:
							// Upper Left Leg
							m_ull_gains = new Gains(kp, kd, UPPER_LEFT_LEG);
							break;
						case 2:
							// Upper right leg
							m_url_gains = new Gains(kp, kd, UPPER_RIGHT_LEG);
							break;
						case 3:
							// Lower Left leg
							m_lll_gains = new Gains(kp, kd, LOWER_LEFT_LEG);
							break;
						case 4:
							// Lower right leg
							m_lrl_gains = new Gains(kp, kd, LOWER_RIGHT_LEG);
							break;
						case 5:
							// Left foot
							m_lf_gains = new Gains(kp, kd, LEFT_FOOT);
							break;
						case 6:
							// Right foot
							m_rf_gains = new Gains(kp, kd, RIGHT_FOOT);
							break;
						default:
							break;
						}
						body++;
					}
				}
			}

		}
	}
	else {
		m_torso_gains = new Gains(0, 0, TORSO);
		m_ull_gains = new Gains(0, 0, UPPER_LEFT_LEG);
		m_url_gains = new Gains(0, 0, UPPER_RIGHT_LEG);
		m_lll_gains = new Gains(0, 0, LOWER_LEFT_LEG);
		m_lrl_gains = new Gains(0, 0, LOWER_RIGHT_LEG);
		m_lf_gains = new Gains(0, 0, LEFT_FOOT);
		m_rf_gains = new Gains(0, 0, RIGHT_FOOT);
	}

	std::vector<Gains *> gains = { m_torso_gains, m_ull_gains, m_url_gains, m_lll_gains, m_lrl_gains, m_lf_gains, m_rf_gains };
	return gains;

}

std::vector<float>WalkingController::ReadFeedbackFile() {
	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string feedback_ext = "fdbk";

		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(feedback_ext, (fname.length() - feedback_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float cd_1, cv_1, cd_2, cv_2;
					char c;
					while ((infile >> cd_1 >> c >> cv_1 >> c >> cd_2 >> c >> cv_2) && (c == ',')) {
						m_cd_1 = cd_1;
						m_cv_1 = cv_1;
						m_cd_2 = cd_2;
						m_cv_2 = cv_2;
					}
				}
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		// Initialize to be zeros
		m_cd_1 = 0.0f;
		m_cv_1 = 0.0f;
		m_cd_2 = 0.0f;
		m_cv_2 = 0.0f;
	}

	std::vector<float> fdbk = {m_cd_1, m_cv_1, m_cd_2, m_cv_2 };
	return fdbk;
}

float WalkingController::ReadTimeFile() {
	DIR *dir;
	struct dirent *ent;
	std::string state_dir = "..\\..\\State Configurations";
	if ((dir = opendir(state_dir.c_str())) != NULL) {
		std::string feedback_ext = "tm";

		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				//printf("%s\n", ent->d_name);
				std::string fname = ent->d_name;
				if (fname.find(feedback_ext, (fname.length() - feedback_ext.length())) != std::string::npos) {
					std::stringstream ss;
					ss << state_dir << "\\" << ent->d_name;
					std::ifstream infile(ss.str());
					float time;
					char c;
					while (infile >> time) {
						m_state_time = time;
					}
				}
			}
		}
		closedir(dir);
	}
	else {
		/* could not open directory */
		// Initialize to be zeros
		m_state_time = 0.0f;
	}

	return m_state_time;
}

void WalkingController::SaveStates() {
	std::ofstream states_file;
	states_file.open("..\\..\\State Configurations\\states.cfg");
	std::vector<State *> states = { m_state0, m_state1, m_state2, m_state3, m_state4 };
	for (std::vector<State *>::iterator it = states.begin(); it != states.end(); it++) {
		char buffer[100];
		sprintf_s(buffer, "%f, %f, %f, %f, %f, %f, %f\n", (*it)->m_torsoAngle, (*it)->m_upperLeftLegAngle, (*it)->m_upperRightLegAngle, (*it)->m_lowerLeftLegAngle, (*it)->m_lowerRightLegAngle, (*it)->m_leftFootAngle, (*it)->m_rightFootAngle);
		std::cout << buffer;
		states_file << buffer;
	}
	states_file.close();
}

void WalkingController::SaveGains() {

	std::ofstream gains_file;
	gains_file.open("..\\..\\State Configurations\\gains.gns");
	std::vector<Gains *> gains = { m_torso_gains, m_ull_gains, m_url_gains, m_lll_gains, m_lrl_gains, m_lf_gains, m_rf_gains };
	for (std::vector<Gains *>::iterator it = gains.begin(); it != gains.end(); it++) {
		char buffer[100];
		sprintf_s(buffer, "%f, %f \n", (*it)->m_kp, (*it)->m_kd);
		std::cout << buffer;
		gains_file << buffer;
	}
	gains_file.close();
}

void WalkingController::SaveFeedback() {
	std::ofstream feedback_file;
	feedback_file.open("..\\..\\State Configurations\\feedbacks.fdbk");
	char buffer[100];
	sprintf_s(buffer, "%f, %f, %f, %f\n", m_cd_1, m_cv_1, m_cd_2, m_cv_2);
	std::cout << buffer;
	feedback_file << buffer;
	feedback_file.close();
}

void WalkingController::SaveTime() {
	std::ofstream time_file;
	time_file.open("..\\..\\State Configurations\\stateTimes.tm");
	char buffer[100];
	sprintf_s(buffer, "%f\n", m_state_time);
	std::cout << buffer;
	time_file << buffer;
	time_file.close();
}

WalkingController::~WalkingController()
{
}

#pragma region WALKER_INTERACTION

void WalkingController::Walk() {

	m_currentState = WALKING;

	switch (m_ragDollState)
	{
	case STATE_0:
	{
		printf("~*~*~*~*~*~*~*~*~*~ STATE 0 ~*~*~*~*~*~*~*~*~*~\n");
		start = std::clock();
		m_ragDollState = STATE_1;
	}
		break;
	case STATE_1:
		printf("~*~*~*~*~*~*~*~*~*~ STATE 1 ~*~*~*~*~*~*~*~*~*~\n");
		m_duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		if (m_duration >= m_state_time) {
			m_ragDollState = STATE_2;
		}
		else {
			// Compute torques for bodies
			std::vector<float> torques = CalculateState1Torques();
			// Apply torques to bodies
			
		}
		break;
	case STATE_2: {
		printf("~*~*~*~*~*~*~*~*~*~ STATE 2 ~*~*~*~*~*~*~*~*~*~\n");
		// Wait until foot contact
	}
		break;
	case STATE_3:
		printf("~*~*~*~*~*~*~*~*~*~ STATE 3 ~*~*~*~*~*~*~*~*~*~\n");
		break;
	case STATE_4:
		printf("~*~*~*~*~*~*~*~*~*~ STATE 4 ~*~*~*~*~*~*~*~*~*~\n");
		break;
	default:
		break;
	}

}

void WalkingController::PauseWalking(){

	m_currentState = PAUSE;

}

void WalkingController::Reset(){
	m_ragDollState = STATE_0;
	m_currentState = RESET;
}

#pragma endregion WALKER_INTERACTION

#pragma region GAINS

void WalkingController::SetTorsoGains(float kp, float kd){
	m_torso_gains->m_kp = kp;
	m_torso_gains->m_kd = kd;
}

void WalkingController::SetUpperLeftLegGains(float kp, float kd){
	m_ull_gains->m_kp = kp;
	m_ull_gains->m_kd = kd;
}

void WalkingController::SetUpperRightLegGains(float kp, float kd){
	m_url_gains->m_kp = kp;
	m_ull_gains->m_kd = kd;
}

void WalkingController::SetLowerLeftLegGains(float kp, float kd){
	m_lll_gains->m_kp = kp;
	m_lll_gains->m_kd = kd;
}

void WalkingController::SetLowerRightLegGains(float kp, float kd){
	m_lrl_gains->m_kp = kp;
	m_lrl_gains->m_kd = kd;
}

void WalkingController::SetLeftFootGains(float kp, float kd){
	m_lf_gains->m_kp = kp;
	m_lf_gains->m_kd = kd;
}

void WalkingController::SetRightFootGains(float kp, float kd){
	m_rf_gains->m_kp = kp;
	m_rf_gains->m_kd = kd;
}

#pragma endregion GAINS

#pragma region STATES

void WalkingController::SetState1(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){

	m_state1 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);
	
}

void WalkingController::SetState2(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){
	m_state2 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);
}

void WalkingController::SetState3(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){

	m_state3 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);

}

void WalkingController::SetState4(float torso, float upperLeftLeg, float upperRightLeg, float lowerLeftLeg, float lowerRightLeg, float leftFoot, float rightFoot){
	
	m_state4 = new State(torso, upperLeftLeg, upperRightLeg, lowerLeftLeg, lowerRightLeg, leftFoot, rightFoot);

}

#pragma endregion STATES

#pragma region CALCULATE_TORQUES

std::vector<float> WalkingController::CalculateState1Torques() {
	// Upper Left leg is the swing hip
	float feedbackTargetAngle = CalculateFeedbackSwingHip();
	//printf("==================== Calculating torque for torso ======================= \n");
	float torsoTorque = CalculateTorqueForTorso(m_state1->m_torsoAngle, m_app->m_torso->GetOrientation(), m_app->m_torso->GetAngularVelocity());
	//printf("==================== Calculating torque for Upper Legs ======================= \n");
	float upperLeftLegTorque = CalculateTorqueForUpperLeftLeg(feedbackTargetAngle, m_app->m_upperLeftLeg->GetOrientation(), m_app->m_upperLeftLeg->GetAngularVelocity());
	float upperRightLegTorque = -torsoTorque - upperLeftLegTorque;
	//printf("==================== Calculating torque for Lower Legs ======================= \n");
	float lowerLeftLegTorque = CalculateTorqueForLowerLeftLeg(m_state1->m_lowerLeftLegAngle, m_app->m_lowerLeftLeg->GetOrientation(), m_app->m_lowerLeftLeg->GetAngularVelocity());
	float lowerRightLegTorque = CalculateTorqueForLowerRightLeg(m_state1->m_lowerRightLegAngle, m_app->m_lowerRightLeg->GetOrientation(), m_app->m_lowerRightLeg->GetAngularVelocity());
	//printf("==================== Calculating torque for Feet ======================= \n");
	float leftFootTorque = CalculateTorqueForLeftFoot(m_state1->m_leftFootAngle, m_app->m_leftFoot->GetOrientation(), m_app->m_leftFoot->GetAngularVelocity());
	float rightFootTorque = CalculateTorqueForRightFoot(m_state1->m_rightFootAngle, m_app->m_rightFoot->GetOrientation(), m_app->m_rightFoot->GetAngularVelocity());

	printf("Torques = %f, %f, %f, %f, %f, %f, %f \n", torsoTorque, upperLeftLegTorque, upperRightLegTorque, lowerLeftLegTorque, lowerRightLegTorque, leftFootTorque, rightFootTorque);

	return std::vector < float > {torsoTorque, upperLeftLegTorque, upperRightLegTorque, lowerLeftLegTorque, lowerRightLegTorque, leftFootTorque, rightFootTorque};
}

std::vector<float> WalkingController::CalculateState2Torques() {
	return std::vector < float > {0};
}
std::vector<float> WalkingController::CalculateState3Torques() {
	return std::vector < float > {0};
}
std::vector<float> WalkingController::CalculateState4Torques() {
	return std::vector < float > {0};
}

float WalkingController::CalculateFeedbackSwingHip() {

	// Swing hip changes between states.
	GameObject *swingHipBody;
	float targetAngle;
	float distance;
	float velocity = m_app->m_torso->GetRigidBody()->getVelocityInLocalPoint(btVector3(-torso_height/2, 0, 0)).x();
	float cd, cv = 0.0f;
	switch (m_ragDollState)
	{
	case STATE_0:
		break;
	case STATE_1: {
		swingHipBody = m_app->m_upperLeftLeg;
		targetAngle = m_state1->m_upperLeftLegAngle;
		distance = abs(Constants::GetInstance().DegreesToRadians(m_state1->m_rightFootAngle - 90) * (foot_width / 4));	
		cd = m_cd_1;
		cv = m_cv_1;
	}
		break;
	case STATE_2: {
		swingHipBody = m_app->m_upperLeftLeg;
		targetAngle = m_state2->m_upperLeftLegAngle;
		distance = abs(Constants::GetInstance().DegreesToRadians(m_state1->m_rightFootAngle - 90) * (foot_width / 4));
		cd = m_cd_2;
		cv = m_cv_2;
	}
		break;
	case STATE_3: {
		swingHipBody = m_app->m_upperRightLeg;
		targetAngle = m_state3->m_upperRightLegAngle;
		distance = abs(Constants::GetInstance().DegreesToRadians(m_state1->m_leftFootAngle - 90) * (foot_width / 4));
		cd = m_cd_1;
		cv = m_cv_1;
	}
		break;
	case STATE_4: {
		swingHipBody = m_app->m_upperRightLeg;
		targetAngle = m_state4->m_upperRightLegAngle;
		distance = abs(Constants::GetInstance().DegreesToRadians(m_state1->m_leftFootAngle - 90) * (foot_width / 4));
		cd = m_cd_2;
		cv = m_cv_2;
	}
		break;
	default:
		break;
	}

	return targetAngle + cd * distance + cv * velocity;

}

float WalkingController::CalculateTorqueForTorso(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_torso_gains->m_kp, m_torso_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForUpperLeftLeg(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_ull_gains->m_kp, m_ull_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForUpperRightLeg(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_url_gains->m_kp, m_url_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForLowerLeftLeg(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_lll_gains->m_kp, m_lll_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForLowerRightLeg(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_lrl_gains->m_kp, m_lrl_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForLeftFoot(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_lf_gains->m_kp, m_lf_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorqueForRightFoot(float targetPosition, float currentPosition, float currentVelocity) {
	return CalculateTorque(m_rf_gains->m_kp, m_rf_gains->m_kd, targetPosition, currentPosition, currentVelocity);
}

float WalkingController::CalculateTorque(float kp, float kd, float targetPosition, float currentPosition, float velocity) {

	return kp * Constants::GetInstance().DegreesToRadians(targetPosition - currentPosition) - kd * Constants::GetInstance().DegreesToRadians(velocity);

}

#pragma endregion CALCULATE_TORQUES